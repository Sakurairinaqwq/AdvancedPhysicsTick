// Copyright AlexanderAL123. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsEngine/BodyInstance.h"
#include "PBDRigidsSolver.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "GameFramework/Pawn.h"
#include "Chaos/Utilities.h"
#include "AdvancedManager.h"
#include "AdvancedAsyncCallback.h"
#include "AdvancedUtility.h"
#include "AdvancedPawn.generated.h"

DECLARE_STATS_GROUP(TEXT("AdvancedPawn"), STATGROUP_AdvancedPawn, STATGROUP_Advanced);

UCLASS(BlueprintType)
class ADVANCEDPLUGIN_API AAdvancedPawn : public APawn
{
	friend class FAdvancedManager;
	friend class FAdvancedAsyncCallback;

	GENERATED_BODY()
public:
	AAdvancedPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/** TODO DESCRIPTION FOR ADVANCEDPLUGIN_API */
	virtual void BeginPlay() override;
	/** TODO DESCRIPTION FOR ADVANCEDPLUGIN_API */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** TODO DESCRIPTION FOR ADVANCEDPLUGIN_API */
	virtual void TickVehicle(float DeltaTime);
	/** TODO DESCRIPTION FOR ADVANCEDPLUGIN_API */
	virtual void PhysicsTick(UWorld* InWorld, float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle);

protected:
	/** Event called PhysicThread */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Advanced Physic Thread"))
	void AdvancedTick(float DeltaTime, float SimTime);

	/** Event called (ChaosScene.PreTick) */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Advanced Game Thread"))
	void AdvancedNativeTick(float DeltaTime);

private:
	/** The main skeletal mesh associated with this Pawn */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = RacingPhysics, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UStaticMeshComponent> Mesh;

	/** The movement component associated with this Pawn */
	UPROPERTY(NoClear)
	UAdvancedMovementComponent* MovementComponent;

public:
	/** TODO DESCRIPTION FOR ADVANCEDPLUGIN_API */
	FBodyInstance* GetBodyInstance() { return Mesh->GetBodyInstance(); }
	/** TODO DESCRIPTION FOR ADVANCEDPLUGIN_API */
	const FBodyInstance* GetBodyInstance() const { return Mesh->GetBodyInstance(); }

public:
	/** TODO DESCRIPTION FOR ADVANCEDPLUGIN_API */
	void ApplyVehicleForces(Chaos::FRigidBodyHandle_Internal* InHandle);

	/**
	 *	TODO DESCRIPTION FOR ADVANCEDPLUGIN_API
	 *  THREADSAFE! CALL THIS FUNCTION ONLY AdvancedTick or PhysicsTick!
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
	void AddForce(const FVector& Force, bool bAllowSubstepping = true, bool bAccelChange = false);

	/**
	 *	TODO DESCRIPTION FOR ADVANCEDPLUGIN_API
	 *  THREADSAFE! CALL THIS FUNCTION ONLY AdvancedTick or PhysicsTick!
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
	void AddForceAtLocation(const FVector& Force, const FVector& Position, bool bAllowSubstepping = true, bool bIsLocalForce = false);

	/**
	 *	TODO DESCRIPTION FOR ADVANCEDPLUGIN_API
	 *  THREADSAFE! CALL THIS FUNCTION ONLY AdvancedTick or PhysicsTick!
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
	void AddImpulse(const FVector& Impulse, bool bVelChange);
	
	/**
	 *	TODO DESCRIPTION FOR ADVANCEDPLUGIN_API
	 *  THREADSAFE! CALL THIS FUNCTION ONLY AdvancedTick or PhysicsTick!
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
	void AddImpulseAtPosition(const FVector& Impulse, const FVector& Position);
	
	/**
	 *	TODO DESCRIPTION FOR ADVANCEDPLUGIN_API
	 *  THREADSAFE! CALL THIS FUNCTION ONLY AdvancedTick or PhysicsTick!
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
	void AddTorqueInRadians(const FVector& Torque, bool bAllowSubstepping = true, bool bAccelChange = false);

	/**
	 *	TODO DESCRIPTION FOR ADVANCEDPLUGIN_API
	 *  THREADSAFE! CALL THIS FUNCTION ONLY AdvancedTick or PhysicsTick!
	 */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
	FVector GetLinearVelocityAtPoint(const UPrimitiveComponent* TargetComponent, const FVector& InPoint);

public:
	/** Returns vehicle world transform */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
	FTransform GetVehicleWorldTransform() const { return VehicleState.VehicleWorldTransform; }

	/** Returns vehicle world velocity */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
	FVector GetVehicleWorldVelocity() const { return VehicleState.VehicleWorldVelocity; }

	/** Returns vehicle world velocity normal */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
	FVector GetVehicleWorldVelocityNormal() const { return VehicleState.VehicleWorldVelocityNormal; }

	/** Returns vehicle world velocity size */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
	float GetVehicleWorldVelocitySize() const { return GetVehicleWorldVelocity().Size(); }

	/** Returns vehicle forward speed */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
	float GetVehicleForwardSpeed() const { return FVector::DotProduct(GetVehicleWorldVelocity(), VehicleState.VehicleXVector); }

	/** Returns vehicle right speed */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
	float GetVehicleRightSpeed() const { return FVector::DotProduct(GetVehicleWorldVelocity(), VehicleState.VehicleYVector); }

	/** Returns vehicle KMH speed */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
	float GetVehicleKMHSpeed() const { return FMath::Abs(GetVehicleForwardSpeed() * 0.036f); }

	/** Returns vehicle MPH speed */
	UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
	float GetVehicleMPHSpeed() const { return FMath::Abs(GetVehicleForwardSpeed() * 0.022369f); }

private:
	UWorld* World;
	Chaos::FRigidBodyHandle_Internal* RigidHandle;

	FVehicleForces VehicleForce;
	FVehicleStatus VehicleState;
};