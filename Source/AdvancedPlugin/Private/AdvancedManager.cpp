// Copyright AlexanderAL123. All Rights Reserved.

#include "AdvancedManager.h"
#include "AdvancedPawn.h"
#include "Engine/World.h"
#include "Physics/Experimental/PhysScene_Chaos.h"
#include "Physics/PhysicsFiltering.h"
#include "Physics/PhysicsInterfaceCore.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "PBDRigidsSolver.h"

DECLARE_CYCLE_STAT(TEXT("RacingManager:Scene_PreUpdate"), STAT_RacingManager_PreUpdate, STATGROUP_RacingManager);
DECLARE_CYCLE_STAT(TEXT("RacingManager:Scene_PostUpdate"), STAT_RacingManager_PostUpdate, STATGROUP_RacingManager);

FDelegateHandle FAdvancedManager::OnPostWorldInitializationHandle;
FDelegateHandle FAdvancedManager::OnWorldCleanupHandle;

TMap<FPhysScene*, FAdvancedManager*> FAdvancedManager::SceneToAdvancedManagerMap;
bool FAdvancedManager::Initialized = false;
uint32 FAdvancedManager::AdvancedSetupTag = 0;

void OnPostWorldInitialization(UWorld* InWorld, const UWorld::InitializationValues)
{
	FAdvancedManager* AManager = FAdvancedManager::GetAdvancedManagerFromScene(InWorld->GetPhysicsScene());
	if (AManager) AManager->RegisterCallbacks();
}

void OnWorldCleanup(UWorld* InWorld, bool bSessionEnded, bool bCleanupResources)
{
	FAdvancedManager* AManager = FAdvancedManager::GetAdvancedManagerFromScene(InWorld->GetPhysicsScene());
	if (AManager) AManager->UnregisterCallbacks();
}

FAdvancedManager::FAdvancedManager(FPhysScene* PhysScene)
	: Scene(*PhysScene), AsyncCallback(nullptr), Timestamp(0)
{
	check(PhysScene);

	if (!Initialized)
	{
		OnPostWorldInitializationHandle = FWorldDelegates::OnPostWorldInitialization.AddStatic(&OnPostWorldInitialization);
		OnWorldCleanupHandle = FWorldDelegates::OnWorldCleanup.AddStatic(&OnWorldCleanup);
		Initialized = true;
	}

	ensure(FAdvancedManager::SceneToAdvancedManagerMap.Find(PhysScene) == nullptr);
	FAdvancedManager::SceneToAdvancedManagerMap.Add(PhysScene, this);
}

FAdvancedManager::~FAdvancedManager()
{
	while (VehicleList.Num() > 0)
	{
		RemoveVehicle(VehicleList.Last());
	}
}

void FAdvancedManager::RegisterCallbacks()
{
	OnPhysScenePreTickHandle = Scene.OnPhysScenePreTick.AddRaw(this, &FAdvancedManager::ScenePreTick);
	OnPhysScenePostTickHandle = Scene.OnPhysScenePostTick.AddRaw(this, &FAdvancedManager::ScenePostTick);

	if (AsyncCallback) return;
	AsyncCallback = Scene.GetSolver()->CreateAndRegisterSimCallbackObject_External<FAdvancedAsyncCallback>();
}

void FAdvancedManager::UnregisterCallbacks()
{
	if (OnPhysScenePreTickHandle.IsValid())
	{
		Scene.OnPhysScenePreTick.Remove(OnPhysScenePreTickHandle);
		OnPhysScenePreTickHandle.Reset();
	}

	if (OnPhysScenePostTickHandle.IsValid())
	{
		Scene.OnPhysScenePostTick.Remove(OnPhysScenePostTickHandle);
		OnPhysScenePostTickHandle.Reset();
	}

	if (AsyncCallback)
	{
		Scene.GetSolver()->UnregisterAndFreeSimCallbackObject_External(AsyncCallback);
		AsyncCallback = nullptr;
	}
}

void FAdvancedManager::ScenePreTick(FPhysScene* PhysScene, float DeltaTime)
{
	SCOPE_CYCLE_COUNTER(STAT_RacingManager_PreUpdate);

	UWorld* World = Scene.GetOwningWorld();

	for (int32 i = 0; i < VehicleList.Num(); ++i)
	{
		VehicleList[i]->TickVehicle(DeltaTime);
	}

	FAdvancedAsyncCallbackInput* AsyncInput = AsyncCallback->GetProducerInputData_External();
	AsyncInput->Reset();
	AsyncInput->VehicleList.Reserve(VehicleList.Num());
	AsyncInput->Timestamp = Timestamp;
	AsyncInput->World = World;

	if (World)
	{
		for (TWeakObjectPtr<AAdvancedPawn> Vehicle : VehicleList)
		{
			if (!Vehicle.IsValid()) continue;
			AsyncInput->VehicleList.Add(Vehicle);
		}
	}

	++Timestamp;
}

void FAdvancedManager::ScenePostTick(FChaosScene* PhysScene)
{
	SCOPE_CYCLE_COUNTER(STAT_RacingManager_PostUpdate);
	
	//TODO: Add vehicle sleeping
}

void FAdvancedManager::AddVehicle(TWeakObjectPtr<AAdvancedPawn> InPawn)
{
	VehicleList.Add(InPawn);
}

void FAdvancedManager::RemoveVehicle(TWeakObjectPtr<AAdvancedPawn> InPawn)
{
	VehicleList.Remove(InPawn);
}

FAdvancedManager* FAdvancedManager::GetAdvancedManagerFromScene(FPhysScene* InPhysScene)
{
	FAdvancedManager** ManagerMap = SceneToAdvancedManagerMap.Find(InPhysScene);
	return ManagerMap ? *ManagerMap : nullptr;
}

void FAdvancedManager::DetachFromPhysScene(FPhysScene* InPhysScene)
{
	UnregisterCallbacks();

	if (InPhysScene->GetOwningWorld())
	{
		InPhysScene->GetOwningWorld()->OnWorldBeginPlay.RemoveAll(this);
	}

	FAdvancedManager::SceneToAdvancedManagerMap.Remove(InPhysScene);
}