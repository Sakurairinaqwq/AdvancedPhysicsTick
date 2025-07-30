// Copyright AlexanderAL123. All Rights Reserved.

#include "AdvancedMovementComponent.h"
#include "AdvancedPawn.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "Chaos/DebugDrawQueue.h"
#include "PhysicsProxy/SuspensionConstraintProxy.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

DECLARE_CYCLE_STAT(TEXT("Advanced Movement:TickVehicle"), STAT_AdvancedPawn_TickVehicle, STATGROUP_AdvancedMovement);
DECLARE_CYCLE_STAT(TEXT("Advanced Movement:PhysicsTick"), STAT_AdvancedPawn_PhysicsTick, STATGROUP_AdvancedMovement);

UAdvancedMovementComponent::UAdvancedMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
}

void UAdvancedMovementComponent::Init(AAdvancedPawn* InPawn)
{
	check(InPawn);
	Pawn = InPawn;
}

void UAdvancedMovementComponent::TickVehicle(float DeltaTime)
{
	//TODO
}

void UAdvancedMovementComponent::PhysicsTick(UWorld* InWorld, float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle)
{
	//TODO
}

void UAdvancedMovementComponent::OnCreatePhysicsState()
{
	Super::OnCreatePhysicsState();

	if (GetWorld()->IsGameWorld())
	{
		FPhysScene* PhysScene = GetWorld()->GetPhysicsScene();
		if (PhysScene)
		{
			Pawn = Cast<AAdvancedPawn>(GetOwner());
		}
	}
}

void UAdvancedMovementComponent::OnDestroyPhysicsState()
{
	Super::OnDestroyPhysicsState();
	Pawn = nullptr;
}

bool UAdvancedMovementComponent::ShouldCreatePhysicsState() const
{
	if (!IsRegistered() || IsBeingDestroyed()) return false;

	if (GetWorld()->IsGameWorld())
	{
		FPhysScene* PhysScene = GetWorld()->GetPhysicsScene();
		if (PhysScene) return true;
	}

	return false;
}