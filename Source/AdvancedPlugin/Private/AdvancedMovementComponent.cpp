// Copyright AlexanderAL123. All Rights Reserved.

#include "AdvancedMovementComponent.h"
#include "AdvancedPawn.h"
#include "Chaos/ChaosEngineInterface.h"
#include "Chaos/PBDSuspensionConstraintData.h"
#include "Chaos/DebugDrawQueue.h"
#include "PhysicsProxy/SuspensionConstraintProxy.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

UAdvancedMovementComponent::UAdvancedMovementComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PrimaryComponentTick.TickGroup = TG_PrePhysics;
	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.SetTickFunctionEnable(true);
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
			Vehicle = Cast<AAdvancedPawn>(GetOwner());
		}
	}
}

void UAdvancedMovementComponent::OnDestroyPhysicsState()
{
	Super::OnDestroyPhysicsState();
	Vehicle = nullptr;
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