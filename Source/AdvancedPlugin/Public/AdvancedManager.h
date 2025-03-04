// Copyright AlexanderAL123. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"
#include "UObject/UObjectIterator.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Chaos/SimCallbackInput.h"
#include "Chaos/SimCallbackObject.h"
#include "AdvancedAsyncCallback.h"

class AAdvancedPawn;

class ADVANCEDPLUGIN_API FAdvancedManager
{
public:
	FAdvancedManager(FPhysScene* InPhysScene);
	~FAdvancedManager();

	/** Register callbacks chaos physics for FAdvancedManager */
	void RegisterCallbacks();
	/** Unregister callbacks chaos physics for FAdvancedManager */
	void UnregisterCallbacks();

	/** Scene.OnPhysScenePreTick */
	void ScenePreTick(FPhysScene* InPhysScene, float DeltaTime);
	/** Scene.OnPhysScenePostTick */
	void ScenePostTick(FChaosScene* InPhysScene);

	/** Add AAdvancedPawn in World */
	void AddVehicle(TWeakObjectPtr<AAdvancedPawn> InPawn);
	/** Remove AAdvancedPawn in World */
	void RemoveVehicle(TWeakObjectPtr<AAdvancedPawn> InPawn);

	/** Find a vehicle callback manager from an FPhysScene */
	static FAdvancedManager* GetAdvancedManagerFromScene(FPhysScene* InPhysScene);

	/** Detach this vehicle manager from a FPhysScene (remove delegates, remove from map etc) */
	void DetachFromPhysScene(FPhysScene* InPhysScene);

	/** Get physic scene (Chaos) */
	FPhysScene_Chaos& GetScene() const { return Scene; }

	static uint32 AdvancedSetupTag;

private:
	static bool Initialized;
	FPhysScene_Chaos& Scene;

	static TMap<FPhysScene*, FAdvancedManager*> SceneToAdvancedManagerMap;
	TArray<TWeakObjectPtr<AAdvancedPawn>> VehicleList;

	FDelegateHandle OnPhysScenePreTickHandle;
	FDelegateHandle OnPhysScenePostTickHandle;

	static FDelegateHandle OnPostWorldInitializationHandle;
	static FDelegateHandle OnWorldCleanupHandle;

	FAdvancedAsyncCallback* AsyncCallback;
	int32 Timestamp;
};