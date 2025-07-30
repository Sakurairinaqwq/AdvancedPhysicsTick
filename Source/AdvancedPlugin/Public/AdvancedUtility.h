// Copyright AlexanderAL123. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

enum class EForcesFlags : uint32
{
	None = 0,
	AllowSubstepping = 1 << 0,
	AccelChange = 1 << 1,
	VelChange = 1 << 2,
	IsLocalForce = 1 << 3
};
ENUM_CLASS_FLAGS(EForcesFlags);

struct ADVANCEDPLUGIN_API FVehicleStatus
{
public:
	FVehicleStatus()
		: VehicleWorldTransform(FTransform::Identity)
		, VehicleWorldVelocity(FVector::ZeroVector)
		, VehicleWorldVelocityNormal(FVector::ZeroVector)
		, VehicleWorldAngularVelocity(FVector::ZeroVector)
		, VehicleWorldCOM(FVector::ZeroVector)
		, VehicleXVector(FVector::ZeroVector)
		, VehicleYVector(FVector::ZeroVector)
		, VehicleZVector(FVector::ZeroVector)
		, VehicleForwardSpeed(0.0f)
		, VehicleRightSpeed(0.0f)
		, VehicleDriftAngle(0.0f)
		, VehicleDriftSide(0.0f)
		, VehicleLocalVelocity(FVector::ZeroVector)
		, VehicleLocalAcceleration(FVector::ZeroVector)
		, VehicleLocalGForce(FVector::ZeroVector)
		, VehicleSleeping(false)
		, VehicleSleepingCounter(0)
		, LastVehicleLocalVelocity(FVector::ZeroVector)
	{
	}

	/** Particle Handle cache capture state */
	void CaptureState(const Chaos::FRigidBodyHandle_Internal* TargetHandle);

	FTransform VehicleWorldTransform;
	FVector VehicleWorldVelocity;
	FVector VehicleWorldVelocityNormal;
	FVector VehicleWorldAngularVelocity;
	FVector VehicleWorldCOM;

	FVector VehicleXVector;   //X Vector
	FVector VehicleYVector;   //Y Vector
	FVector VehicleZVector;   //Z Vector

	float VehicleForwardSpeed;
	float VehicleRightSpeed;

	float VehicleDriftAngle;
	float VehicleDriftSide;

	FVector VehicleLocalVelocity;
	FVector VehicleLocalAcceleration;
	FVector VehicleLocalGForce;

	bool VehicleSleeping;
	int VehicleSleepingCounter;

private:
	FVector LastVehicleLocalVelocity;
};

class ADVANCEDPLUGIN_API FVehicleForces
{
public:
	struct FRacingVehicleApplyForce_DATA
	{
		FRacingVehicleApplyForce_DATA(const FVector& ForceIn, bool bAllowSubsteppingIn, bool bAccelChangeIn)
			: Force(ForceIn), Flags(EForcesFlags::None)
		{
			Flags |= bAllowSubsteppingIn ? EForcesFlags::AllowSubstepping : EForcesFlags::None;
			Flags |= bAccelChangeIn ? EForcesFlags::AccelChange : EForcesFlags::None;
		}

		FVector Force;
		EForcesFlags Flags;
	};

	struct FRacingVehicleApplyForceAtPosition_DATA
	{
		FRacingVehicleApplyForceAtPosition_DATA(const FVector& ForceIn, const FVector& PositionIn, bool bAllowSubsteppingIn, bool bIsLocalForceIn)
			: Force(ForceIn), Position(PositionIn), Flags(EForcesFlags::None)
		{
			Flags |= bAllowSubsteppingIn ? EForcesFlags::AllowSubstepping : EForcesFlags::None;
			Flags |= bIsLocalForceIn ? EForcesFlags::IsLocalForce : EForcesFlags::None;
		}

		FVector Force;
		FVector Position;
		EForcesFlags Flags;
	};

	struct FRacingVehicleAddTorqueInRadians_DATA
	{
		FRacingVehicleAddTorqueInRadians_DATA(const FVector& TorqueIn, bool bAllowSubsteppingIn, bool bAccelChangeIn)
			: Torque(TorqueIn), Flags(EForcesFlags::None)
		{
			Flags |= bAllowSubsteppingIn ? EForcesFlags::AllowSubstepping : EForcesFlags::None;
			Flags |= bAccelChangeIn ? EForcesFlags::AccelChange : EForcesFlags::None;
		}

		FVector Torque;
		EForcesFlags Flags;
	};

	struct FRacingVehicleAddImpulse_DATA
	{
		FRacingVehicleAddImpulse_DATA(const FVector& ImpulseIn, const bool bVelChangeIn)
			: Impulse(ImpulseIn), Flags(EForcesFlags::None)
		{
			Flags |= bVelChangeIn ? EForcesFlags::VelChange : EForcesFlags::None;
		}

		FVector Impulse;
		EForcesFlags Flags;
	};

	struct FRacingVehicleAddImpulseAtPosition_DATA
	{
		FRacingVehicleAddImpulseAtPosition_DATA(const FVector& ImpulseIn, const FVector& PositionIn)
			: Impulse(ImpulseIn), Position(PositionIn)
		{
		}

		FVector Impulse;
		FVector Position;
	};

	void Add(const FRacingVehicleApplyForce_DATA& ApplyForceDataIn)
	{
		ApplyForceDatas.Add(ApplyForceDataIn);
	}

	void Add(const FRacingVehicleApplyForceAtPosition_DATA& ApplyForceAtPositionDataIn)
	{
		ApplyForceAtPositionDatas.Add(ApplyForceAtPositionDataIn);
	}

	void Add(const FRacingVehicleAddTorqueInRadians_DATA& ApplyTorqueDataIn)
	{
		ApplyTorqueDatas.Add(ApplyTorqueDataIn);
	}

	void Add(const FRacingVehicleAddImpulse_DATA& ApplyImpulseDataIn)
	{
		ApplyImpulseDatas.Add(ApplyImpulseDataIn);
	}

	void Add(const FRacingVehicleAddImpulseAtPosition_DATA& ApplyImpulseAtPositionDataIn)
	{
		ApplyImpulseAtPositionDatas.Add(ApplyImpulseAtPositionDataIn);
	}

	void Apply(Chaos::FRigidBodyHandle_Internal* RigidHandle)
	{
		for (const FRacingVehicleApplyForce_DATA& Data : ApplyForceDatas)
		{
			AddForce(RigidHandle, Data);
		}

		for (const FRacingVehicleApplyForceAtPosition_DATA& Data : ApplyForceAtPositionDatas)
		{
			AddForceAtPosition(RigidHandle, Data);
		}

		for (const FRacingVehicleAddTorqueInRadians_DATA& Data : ApplyTorqueDatas)
		{
			AddTorque(RigidHandle, Data);
		}

		for (const FRacingVehicleAddImpulse_DATA& Data : ApplyImpulseDatas)
		{
			AddImpulse(RigidHandle, Data);
		}

		for (const FRacingVehicleAddImpulseAtPosition_DATA& Data : ApplyImpulseAtPositionDatas)
		{
			AddImpulseAtPosition(RigidHandle, Data);
		}

		ApplyForceDatas.Empty();
		ApplyForceAtPositionDatas.Empty();
		ApplyTorqueDatas.Empty();
		ApplyImpulseDatas.Empty();
		ApplyImpulseAtPositionDatas.Empty();
	}

	static FVector GetVelocityAtPoint(const FBodyInstance* Body, const FVector& PointIn);
	static FVector GetVelocityAtPoint(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FVector& PointIn);

private:
	void AddForce(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleApplyForce_DATA& DataIn);
	void AddForceAtPosition(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleApplyForceAtPosition_DATA& DataIn);
	void AddTorque(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleAddTorqueInRadians_DATA& DataIn);
	void AddImpulse(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleAddImpulse_DATA& DataIn);
	void AddImpulseAtPosition(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleAddImpulseAtPosition_DATA& DataIn);

	TArray<FRacingVehicleApplyForce_DATA> ApplyForceDatas;
	TArray<FRacingVehicleApplyForceAtPosition_DATA> ApplyForceAtPositionDatas;
	TArray<FRacingVehicleAddTorqueInRadians_DATA> ApplyTorqueDatas;
	TArray<FRacingVehicleAddImpulse_DATA> ApplyImpulseDatas;
	TArray<FRacingVehicleAddImpulseAtPosition_DATA> ApplyImpulseAtPositionDatas;
};