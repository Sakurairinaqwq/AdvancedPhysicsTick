// Copyright AlexanderAL123. All Rights Reserved.

#include "AdvancedAsyncCallback.h"
#include "AdvancedPawn.h"
#include "Chaos/ParticleHandleFwd.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"
#include "PBDRigidsSolver.h"

FAdvancedDebugParams GAdvVehicleDebugParams;

FAutoConsoleVariableRef CVarRacingVehicleManagerPhysicsUpdate(TEXT("p.RacingVehicle.PhysicsUpdate"), GAdvVehicleDebugParams.PhysicsUpdate, TEXT("Enable/Disable physics simulation update. Userful for debugging"));
FAutoConsoleVariableRef CVarRacingVehicleManagerMultithreading(TEXT("p.RacingVehicle.Multithreading"), GAdvVehicleDebugParams.ForceMultithreading, TEXT("Enable multi-threading of vehicles updates."));

DECLARE_CYCLE_STAT(TEXT("RacingManager:Async_ProcessInputs"), STAT_RacingManager_OnProcessInputs, STATGROUP_RacingManager);
DECLARE_CYCLE_STAT(TEXT("RacingManager:Async_PreSimulate"), STAT_RacingManager_OnPreSimulate, STATGROUP_RacingManager);

void FAdvancedAsyncCallback::ProcessInputs_Internal(int32 PhysicsStep)
{
	SCOPE_CYCLE_COUNTER(STAT_RacingManager_OnProcessInputs);

	int32 PStep = PhysicsStep;
}

void FAdvancedAsyncCallback::OnPreSimulate_Internal()
{
	SCOPE_CYCLE_COUNTER(STAT_RacingManager_OnPreSimulate);

	float DeltaTime = GetDeltaTime_Internal();
	float SimTime = GetSimTime_Internal();

	const FAdvancedAsyncCallbackInput* Input = GetConsumerInput_Internal();
	if (!Input) return;

	UWorld* World = Input->World.Get();
	if (!World) return;

	const int32 NumVehicles = Input->VehicleList.Num();
	if (NumVehicles == 0) return;

	FAdvancedAsyncCallbackOutput& Output = GetProducerOutputData_Internal();
	Output.Timestamp = Input->Timestamp;

	if (!GAdvVehicleDebugParams.PhysicsUpdate) return;
	
	bool ForceSingleThread = !GAdvVehicleDebugParams.ForceMultithreading;
	Chaos::PhysicsParallelFor(Input->VehicleList.Num(), [World, DeltaTime, SimTime, &Input](int32 Idx)
		{
			auto& Vehicle = Input->VehicleList[Idx];
			if (!Vehicle.IsValid()) return;

			if (const auto Handle = Vehicle->GetBodyInstance()->ActorHandle)
			{
				if (Chaos::FRigidBodyHandle_Internal* InHandle = Handle->GetPhysicsThreadAPI())
				{
					Vehicle->PhysicsTick(World, DeltaTime, SimTime, InHandle);
				}
			}
		}, ForceSingleThread);

	for (const auto& Vehicle : Input->VehicleList)
	{
		if (Vehicle.IsValid())
		{
			if (const auto Handle = Vehicle->GetBodyInstance()->ActorHandle)
			{
				if (Chaos::FRigidBodyHandle_Internal* InHandle = Handle->GetPhysicsThreadAPI())
				{
					Vehicle->ApplyVehicleForces(InHandle);
				}
			}
		}
	}
}