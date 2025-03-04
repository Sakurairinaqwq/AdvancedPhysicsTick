// Copyright AlexanderAL123. All Rights Reserved.

#include "AdvancedPlugin.h"
#include "AdvancedManager.h"

IMPLEMENT_MODULE(FAdvancedPluginModule, AdvancedPlugin)

void FAdvancedPluginModule::PhysSceneInit(FPhysScene* PhysScene)
{
	new FAdvancedManager(PhysScene);
}

void FAdvancedPluginModule::PhysSceneTerm(FPhysScene* PhysScene)
{
	FAdvancedManager* AdvancedManager = FAdvancedManager::GetAdvancedManagerFromScene(PhysScene);
	if (!AdvancedManager) return;
	AdvancedManager->DetachFromPhysScene(PhysScene);
	delete AdvancedManager;
	AdvancedManager = nullptr;
}

void FAdvancedPluginModule::StartupModule()
{
	OnPhysSceneInitHandle = FPhysicsDelegates::OnPhysSceneInit.AddRaw(this, &FAdvancedPluginModule::PhysSceneInit);
	OnPhysSceneTermHandle = FPhysicsDelegates::OnPhysSceneTerm.AddRaw(this, &FAdvancedPluginModule::PhysSceneTerm);
}

void FAdvancedPluginModule::ShutdownModule()
{
	FPhysicsDelegates::OnPhysSceneInit.Remove(OnPhysSceneInitHandle);
	FPhysicsDelegates::OnPhysSceneTerm.Remove(OnPhysSceneTermHandle);
}