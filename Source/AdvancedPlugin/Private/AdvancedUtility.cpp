// Copyright AlexanderAL123. All Rights Reserved.

#include "AdvancedUtility.h"
#include "Chaos/Particle/ParticleUtilities.h"
#include "Chaos/Utilities.h"
#include "PhysicsProxy/SingleParticlePhysicsProxy.h"

static float VEHICLE_MAX_DRIFT_ANGLE = 90.0f;

void FVehicleStatus::CaptureState(float DeltaTime, float GravityZ, const Chaos::FRigidBodyHandle_Internal* TargetHandle)
{
	if (!TargetHandle) return;

	const FTransform WorldTM(TargetHandle->R(), TargetHandle->X());
	VehicleWorldTransform = WorldTM;
	VehicleWorldVelocity = TargetHandle->V();
	VehicleWorldVelocityNormal = VehicleWorldVelocity.GetSafeNormal();
	VehicleWorldAngularVelocity = TargetHandle->W();
	VehicleWorldCOM = TargetHandle->CenterOfMass();

	VehicleXVector = VehicleWorldTransform.GetUnitAxis(EAxis::X);
	VehicleYVector = VehicleWorldTransform.GetUnitAxis(EAxis::Y);
	VehicleZVector = VehicleWorldTransform.GetUnitAxis(EAxis::Z);

	VehicleForwardSpeed = FVector::DotProduct(VehicleWorldVelocity, VehicleXVector);
	VehicleRightSpeed = FVector::DotProduct(VehicleWorldVelocity, VehicleYVector);

	VehicleDriftAngle = VehicleForwardSpeed > 400.0f ? FMath::Clamp(FMath::RadiansToDegrees(FMath::Atan2(VehicleRightSpeed, VehicleForwardSpeed)), -VEHICLE_MAX_DRIFT_ANGLE, VEHICLE_MAX_DRIFT_ANGLE) : 0.0f;
	VehicleDriftSide = FMath::Sign(VehicleDriftAngle);

	VehicleLocalVelocity = VehicleWorldTransform.InverseTransformVector(VehicleWorldVelocity);
	VehicleLocalAcceleration = (VehicleLocalVelocity - LastVehicleLocalVelocity) / DeltaTime;
	VehicleLocalGForce = FMath::Abs(GravityZ) > 0.0f ? VehicleLocalAcceleration / FMath::Abs(GravityZ) : FVector::ZeroVector;

	LastVehicleLocalVelocity = VehicleLocalVelocity;
}

void FVehicleForces::AddForce(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleApplyForce_DATA& DataIn)
{
	if (ensure(RigidHandle))
	{
		Chaos::EObjectStateType ObjectState = RigidHandle->ObjectState();
		if (CHAOS_ENSURE(ObjectState == Chaos::EObjectStateType::Dynamic || ObjectState == Chaos::EObjectStateType::Sleeping))
		{
			if ((DataIn.Flags & EForcesFlags::AccelChange) == EForcesFlags::AccelChange)
			{
				const float RigidMass = RigidHandle->M();
				const Chaos::FVec3 Acceleration = DataIn.Force * RigidMass;
				RigidHandle->AddForce(Acceleration, false);
			}
			else
			{
				RigidHandle->AddForce(DataIn.Force, false);
			}
		}
	}
}

void FVehicleForces::AddForceAtPosition(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleApplyForceAtPosition_DATA& DataIn)
{
	if (ensure(RigidHandle))
	{
		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle);
		const Chaos::FVec3 WorldTorque = Chaos::FVec3::CrossProduct(DataIn.Position - WorldCOM, DataIn.Force);
		RigidHandle->AddForce(DataIn.Force, false);
		RigidHandle->AddTorque(WorldTorque, false);
	}
}

void FVehicleForces::AddTorque(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleAddTorqueInRadians_DATA& DataIn)
{
	if (ensure(RigidHandle))
	{
		Chaos::EObjectStateType ObjectState = RigidHandle->ObjectState();

		if (CHAOS_ENSURE(ObjectState == Chaos::EObjectStateType::Dynamic || ObjectState == Chaos::EObjectStateType::Sleeping))
		{
			if ((DataIn.Flags & EForcesFlags::AccelChange) == EForcesFlags::AccelChange)
			{
				RigidHandle->AddTorque(Chaos::FParticleUtilitiesXR::GetWorldInertia(RigidHandle) * DataIn.Torque, false);
			}
			else
			{
				RigidHandle->AddTorque(DataIn.Torque, false);
			}
		}
	}
}

void FVehicleForces::AddImpulse(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleAddImpulse_DATA& DataIn)
{
	if (ensure(RigidHandle))
	{
		if ((DataIn.Flags & EForcesFlags::VelChange) == EForcesFlags::VelChange)
		{
			RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + RigidHandle->M() * DataIn.Impulse, false);
		}
		else
		{
			RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + DataIn.Impulse, false);
		}
	}
}

void FVehicleForces::AddImpulseAtPosition(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FRacingVehicleAddImpulseAtPosition_DATA& DataIn)
{
	if (ensure(RigidHandle))
	{
		const Chaos::FVec3 WorldCOM = Chaos::FParticleUtilitiesGT::GetCoMWorldPosition(RigidHandle);
		const Chaos::FVec3 AngularImpulse = Chaos::FVec3::CrossProduct(DataIn.Position - WorldCOM, DataIn.Impulse);
		RigidHandle->SetLinearImpulse(RigidHandle->LinearImpulse() + DataIn.Impulse, false);
		RigidHandle->SetAngularImpulse(RigidHandle->AngularImpulse() + AngularImpulse, false);
	}
}

FVector FVehicleForces::GetVelocityAtPoint(const FBodyInstance* Body, const FVector& InPoint)
{
	if (FPhysicsActorHandle ActorHandle = Body->ActorHandle)
	{
		if (!IsInGameThread())
		{
			if (Chaos::FRigidBodyHandle_Internal* RigidHandle = ActorHandle->GetPhysicsThreadAPI())
			{
				if (ensure(RigidHandle))
				{
					const Chaos::FVec3 COM = RigidHandle->X() + RigidHandle->R() * RigidHandle->CenterOfMass();
					const Chaos::FVec3 Diff = InPoint - COM;
					return RigidHandle->V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle->W());
				}
			}
			return FVector::ZeroVector;
		}
		else
		{
			Chaos::FRigidBodyHandle_External& RigidHandle = ActorHandle->GetGameThreadAPI();
			const Chaos::FVec3 COM = RigidHandle.X() + RigidHandle.R() * RigidHandle.CenterOfMass();
			const Chaos::FVec3 Diff = InPoint - COM;
			return RigidHandle.V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle.W());
		}
	}
	return FVector::ZeroVector;
}

FVector FVehicleForces::GetVelocityAtPoint(Chaos::FRigidBodyHandle_Internal* RigidHandle, const FVector& PointIn)
{
	if (ensure(RigidHandle))
	{
		const Chaos::FVec3 COM = RigidHandle->X() + RigidHandle->R() * RigidHandle->CenterOfMass();
		const Chaos::FVec3 Diff = PointIn - COM;
		return RigidHandle->V() - Chaos::FVec3::CrossProduct(Diff, RigidHandle->W());
	}
	return FVector::ZeroVector;
}