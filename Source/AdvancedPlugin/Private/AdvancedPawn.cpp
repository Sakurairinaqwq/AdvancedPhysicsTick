// Copyright AlexanderAL123. All Rights Reserved.

#include "AdvancedPawn.h"
#include "AdvancedMovementComponent.h"
#include "Components/StaticMeshComponent.h"
#include "PhysicsEngine/PhysicsSettings.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Chaos/Particle/ParticleUtilities.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "Chaos/DebugDrawQueue.h"
#include "DrawDebugHelpers.h"
#include "DisplayDebugHelpers.h"

extern FAdvancedDebugParams GAdvVehicleDebugParams;

static FAutoConsoleVariableRef CVarRacingVehicleDrawAllForces(TEXT("p.RacingVehicle.AdvPawn.DrawAllForces"), GAdvVehicleDebugParams.DrawDebugAllForces,
	TEXT("Show debug draw"),
	FConsoleVariableDelegate::CreateLambda([](IConsoleVariable* Var)
		{
			IConsoleVariable* ChaosDebugCVar = IConsoleManager::Get().FindConsoleVariable(TEXT("p.Chaos.DebugDraw.Enabled"));
			if (ChaosDebugCVar)
			{
				if (Var->GetInt() > 0) ChaosDebugCVar->Set(1, ECVF_SetByConsole); // Enable
				else ChaosDebugCVar->Set(0, ECVF_SetByConsole); // Disable
			}
		})
);

static FAutoConsoleVariableRef CVarRacingVehicleDrawDebugTextForces(TEXT("p.RacingVehicle.AdvPawn.DrawDebugTextForces"), GAdvVehicleDebugParams.DrawDebugTextForces, TEXT("TODO"));
static FAutoConsoleVariableRef CVarRacingVehicleDrawDebugLinesSize(TEXT("p.RacingVehicle.AdvPawn.DrawDebugLinesSize"), GAdvVehicleDebugParams.DrawDebugLinesSize, TEXT("TODO"));

DECLARE_CYCLE_STAT(TEXT("AdvancedPawn:TickVehicle"), STAT_AdvancedPawn_TickVehicle, STATGROUP_AdvancedPawn);
DECLARE_CYCLE_STAT(TEXT("AdvancedPawn:PhysicsTick"), STAT_AdvancedPawn_PhysicsTick, STATGROUP_AdvancedPawn);

AAdvancedPawn::AAdvancedPawn(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("VehicleMesh"));
	Mesh->BodyInstance.bSimulatePhysics = true;
	Mesh->BodyInstance.bNotifyRigidBodyCollision = true;
	Mesh->BodyInstance.bUseCCD = true;
	Mesh->BodyInstance.bIgnoreAnalyticCollisions = false;
	Mesh->BodyInstance.SetInertiaConditioningEnabled(false);
	Mesh->bApplyImpulseOnDamage = 0;
	Mesh->SetCollisionProfileName(UCollisionProfile::Vehicle_ProfileName);
	Mesh->SetGenerateOverlapEvents(true);
	Mesh->SetCanEverAffectNavigation(false);
	Mesh->SetHiddenInSceneCapture(true);
	Mesh->bCastContactShadow = false;
	Mesh->bRenderInDepthPass = false;
	Mesh->bReceivesDecals = false;
	Mesh->bRenderCustomDepth = true;
	Mesh->CustomDepthStencilValue = 5;
	RootComponent = Mesh;
}

void AAdvancedPawn::BeginPlay()
{
	Super::BeginPlay();

	MovementComponent = FindComponentByClass<UAdvancedMovementComponent>();
	if (MovementComponent) MovementComponent->Init(this);

	if (GetWorld()->IsGameWorld())
	{
		FPhysScene* PhysScene = GetWorld()->GetPhysicsScene();
		if (PhysScene && FAdvancedManager::GetAdvancedManagerFromScene(PhysScene))
		{
			VehicleState.CaptureState(GetBodyInstance());
			FAdvancedManager* AManager = FAdvancedManager::GetAdvancedManagerFromScene(PhysScene);
			AManager->AddVehicle(this);
		}
	}
}

void AAdvancedPawn::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	FAdvancedManager* AManager = FAdvancedManager::GetAdvancedManagerFromScene(this->GetWorld()->GetPhysicsScene());
	AManager->RemoveVehicle(this);
}

void AAdvancedPawn::TickVehicle(float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedPawn_TickVehicle);
	AdvancedNativeTick(DeltaTime);
}

void AAdvancedPawn::PhysicsTick(UWorld* InWorld, float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle)
{
	SCOPE_CYCLE_COUNTER(STAT_AdvancedPawn_PhysicsTick);

	World = InWorld;
	RigidHandle = InHandle;

	if (!World || !RigidHandle) return;

	VehicleState.CaptureState(RigidHandle);
	AdvancedTick(DeltaTime, SimTime);

	if (MovementComponent) MovementComponent->PhysicsTick();
}

void AAdvancedPawn::ApplyVehicleForces(Chaos::FRigidBodyHandle_Internal* InHandle)
{
	VehicleForce.Apply(InHandle);
}

void AAdvancedPawn::AddForce(const FVector& Force, bool bAllowSubstepping, bool bAccelChange)
{
	VehicleForce.Add(FVehicleForces::FRacingVehicleApplyForce_DATA(Force, bAllowSubstepping, bAccelChange));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (GAdvVehicleDebugParams.DrawDebugAllForces)
	{
		FVector Position = RigidHandle->X();
		Chaos::FDebugDrawQueue::GetInstance().DrawDebugDirectionalArrow(Position, Position + Force * GAdvVehicleDebugParams.DrawDebugLinesSize, 20.0f, FColor::White, false, 0, SDPG_World, 2.0f);
	}
#endif
}

void AAdvancedPawn::AddForceAtLocation(const FVector& Force, const FVector& Position, bool bAllowSubstepping, bool bIsLocalForce)
{
	VehicleForce.Add(FVehicleForces::FRacingVehicleApplyForceAtPosition_DATA(Force, Position, bAllowSubstepping, bIsLocalForce));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (GAdvVehicleDebugParams.DrawDebugAllForces)
	{
		Chaos::FDebugDrawQueue::GetInstance().DrawDebugDirectionalArrow(Position, Position + Force * GAdvVehicleDebugParams.DrawDebugLinesSize, 20.0f, FColor::White, false, 0, SDPG_World, 2.0f);
	}
#endif
}

void AAdvancedPawn::AddImpulse(const FVector& Impulse, bool bVelChange)
{
	VehicleForce.Add(FVehicleForces::FRacingVehicleAddImpulse_DATA(Impulse, bVelChange));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (GAdvVehicleDebugParams.DrawDebugAllForces)
	{
		FVector Position = VehicleState.VehicleWorldCOM;
		Chaos::FDebugDrawQueue::GetInstance().DrawDebugDirectionalArrow(Position, Position + Impulse * GAdvVehicleDebugParams.DrawDebugLinesSize, 20.0f, FColor::Red, false, 0, SDPG_World, 2.0f);
	}
#endif
}

void AAdvancedPawn::AddImpulseAtPosition(const FVector& Impulse, const FVector& Position)
{
	VehicleForce.Add(FVehicleForces::FRacingVehicleAddImpulseAtPosition_DATA(Impulse, Position));

#if !(UE_BUILD_SHIPPING || UE_BUILD_TEST)
	if (GAdvVehicleDebugParams.DrawDebugAllForces)
	{
		Chaos::FDebugDrawQueue::GetInstance().DrawDebugDirectionalArrow(Position, Position + Impulse * GAdvVehicleDebugParams.DrawDebugLinesSize, 20.0f, FColor::Red, false, 0, SDPG_World, 2.0f);
	}
#endif
}

void AAdvancedPawn::AddTorqueInRadians(const FVector& Torque, bool bAllowSubstepping, bool bAccelChange)
{
	VehicleForce.Add(FVehicleForces::FRacingVehicleAddTorqueInRadians_DATA(Torque, bAllowSubstepping, bAccelChange));
}

FVector AAdvancedPawn::GetLinearVelocityAtPoint(const UPrimitiveComponent* TargetComponent, const FVector& InPoint)
{
	if (TargetComponent)
	{
		if (FBodyInstance* BodyInstance = TargetComponent->GetBodyInstance())
		{
			return FVehicleForces::GetVelocityAtPoint(BodyInstance, InPoint);
		}
	}

	return FVector::ZeroVector;
}