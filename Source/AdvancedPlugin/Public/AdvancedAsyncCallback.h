// Copyright AlexanderAL123. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "PhysicsPublic.h"
#include "Chaos/GeometryParticlesfwd.h"
#include "Chaos/SimCallbackInput.h"
#include "Chaos/SimCallbackObject.h"

class AAdvancedPawn;

DECLARE_STATS_GROUP(TEXT("RacingManager"), STATGROUP_RacingManager, STATGROUP_Advanced);

struct ADVANCEDPLUGIN_API FAdvancedDebugParams
{
	float DrawDebugLinesSize = 0.0005f;
	bool DrawDebugAllForces = false;
	bool DrawDebugTextForces = false;
	bool PhysicsUpdate = true;
	bool ForceMultithreading = false;
};

struct ADVANCEDPLUGIN_API FAdvancedAsyncCallbackInput : public Chaos::FSimCallbackInput
{
	int32 Timestamp = INDEX_NONE;
	TArray<TWeakObjectPtr<AAdvancedPawn>> VehicleList;
	TWeakObjectPtr<UWorld> World;

	void Reset()
	{
		VehicleList.Reset();
		World.Reset();
	}
};

struct ADVANCEDPLUGIN_API FAdvancedAsyncCallbackOutput : public Chaos::FSimCallbackOutput
{
	int32 Timestamp = INDEX_NONE;
	void Reset() {}
};

class ADVANCEDPLUGIN_API FAdvancedAsyncCallback : public Chaos::TSimCallbackObject<FAdvancedAsyncCallbackInput, FAdvancedAsyncCallbackOutput, Chaos::ESimCallbackOptions::Presimulate | Chaos::ESimCallbackOptions::Rewind>
{
private:
	virtual void ProcessInputs_Internal(int32 PhysicsStep) override;
	virtual void OnPreSimulate_Internal() override;
};