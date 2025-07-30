// Copyright AlexanderAL123. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "Templates/SubclassOf.h"
#include "PhysicsProxy/SingleParticlePhysicsProxyFwd.h"
#include "GameFramework/PawnMovementComponent.h"
#include "AdvancedMovementComponent.generated.h"

DECLARE_STATS_GROUP(TEXT("AdvancedMovement"), STATGROUP_AdvancedMovement, STATGROUP_Advanced);

class AAdvancedPawn;

UCLASS(meta = (BlueprintSpawnableComponent), HideCategories = (ComponentTick, PlanarMovement, Activation, Velocity, ComponentReplication, Cooking))
class ADVANCEDPLUGIN_API UAdvancedMovementComponent : public UPawnMovementComponent
{
	GENERATED_UCLASS_BODY()
public:
	/** */
	virtual void Init(AAdvancedPawn* InPawn);
	/** Tick for vehicle tuning and other state such as user input. */
	virtual void TickVehicle(float DeltaTime);
	/** Physics tick for advanced movement component */
	virtual void PhysicsTick(UWorld* InWorld, float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle);

	/** Used to create any physics engine information for this component */
	virtual void OnCreatePhysicsState() override;
	/** Used to shutdown and physics engine structure for this component */
	virtual void OnDestroyPhysicsState() override;
	/** Return true if it's suitable to create a physics representation of the vehicle at this time */
	virtual bool ShouldCreatePhysicsState() const override;
	///** Returns true if the physics state exists */
	//virtual bool HasValidPhysicsState() const override;
	///** Stops movement immediately (zeroes velocity, usually zeros acceleration for components with acceleration) */
	//virtual void StopMovementImmediately() override;

#if WITH_EDITOR
	///** Called by the editor to query whether a property of this object is allowed to be modified. */
	//virtual bool CanEditChange(const FProperty* Property) const override;
	///** Respond to a property change in editor */
	//virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

private:
	AAdvancedPawn* Pawn;
};