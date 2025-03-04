Advanced Physics Tick
=====================

<div align="center">
    <img src="Resources/Docs/Banner.png" alt="Banner" width="800">
</div>

Advanced Physics Tick is a plugin for Unreal Engine 5 designed to work with asynchronous Chaos physics. It is specifically focused on vehicle simulation and includes a single UStaticMesh (VehicleMesh) that provides more robust and improved control over the physics model. This plugin is not a general-purpose plugin, but is designed with an emphasis on accurate and flexible motion simulation for Vehicles.

Setup
=====================

- Clone or download the repository from GitHub:
```
git clone https://github.com/your-repo/AdvancedPhysicsTick.git
```
- Place the plugin folder in the plugins directory of your Unreal Engine 5 project.
- Run Unreal Engine 5 project.

Documentation
=====================

### 1. AddForce, AddForceAtLocation and etc...

-   Use these functions to interact with VehicleMesh in AAdvancedPawn

	> THREADSAFE! CALL THIS FUNCTIONS ONLY AdvancedTick in BP or PhysicsTick in C++!
```
UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
void AddForce(const FVector& Force, bool bAllowSubstepping = true, bool bAccelChange = false);

UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
void AddForceAtLocation(const FVector& Force, const FVector& Position, bool bAllowSubstepping = true, bool bIsLocalForce = false);

UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
void AddImpulse(const FVector& Impulse, bool bVelChange);

UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
void AddTorqueInRadians(const FVector& Torque, bool bAllowSubstepping = true, bool bAccelChange = false);
```

---

-   Use VehicleState for get transform and velocity in VehicleMesh

    > CALL ANYWHERE!

```
UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
FTransform GetVehicleWorldTransform() const { return VehicleState.VehicleWorldTransform; }

UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
FVector GetVehicleWorldVelocity() const { return VehicleState.VehicleWorldVelocity; }

UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
FVector GetVehicleWorldVelocityNormal() const { return VehicleState.VehicleWorldVelocityNormal; }

UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
float GetVehicleWorldVelocitySize() const { return GetVehicleWorldVelocity().Size(); }

UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
float GetVehicleForwardSpeed() const { return FVector::DotProduct(GetVehicleWorldVelocity(), VehicleState.VehicleXVector); }

UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|State")
float GetVehicleRightSpeed() const { return FVector::DotProduct(GetVehicleWorldVelocity(), VehicleState.VehicleYVector); }
```

---
Blueprint graph implementation

<div align="left">
    <img src="Resources/Docs/BPGraph.png" alt="BPGraph" width="1000">
</div>

### 2. GetLinearVelocityAtPoint.

```
UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
FVector GetLinearVelocityAtPoint(const UPrimitiveComponent* TargetComponent, const FVector& InPoint);
```
#### Params:
-   `InComponent`: Component to which the force will be applied with bSimulatePhysics = true.
-   `InPoint`: Point velocity.

	> THREADSAFE! CALL THIS FUNCTION ONLY AdvancedTick in BP or PhysicsTick in C++!
---

### 3. AdvancedTick and AdvancedNativeTick.

You can override TickVehicle and PhysicsTick in your class for advanced physics logic:

**TickVehicle (AdvancedNativeTick)**
```
virtual void TickVehicle(float DeltaTime) override;
```
Parameters:
-   `DeltaTime`: GameThread, userful for control wake/sleeping vehicle mesh, visual effects, sounds and animations.

    > AdvancedNativeTick can sometimes be called twice and also has a minimum DeltaTime of 1/20.

---

**PhysicsTick (AdvancedTick)**
```
virtual void PhysicsTick(UWorld* InWorld, float DeltaTime, float SimTime, Chaos::FRigidBodyHandle_Internal* InHandle) override;
```
Parameters:
-   `DeltaTime`: PhysicThread, userful for simulation and apply forces.
-   `SimTime`: Current simulation time.

---
Blueprint graph implementation
<div align="left">
    <img src="Resources/Docs/BPEvents.png" alt="BPEvents" width="600">
</div>

### 3. Stat and Debugging
Unfortunately, you can't currently use DrawDebugHelpers in a physics thread, otherwise it will crash the UnrealEngine or cause HandledEnsure type errors.
For PhysicThread use in C++
Chaos::FDebugDrawQueue::GetInstance().DrawDebug.....
or for Blueprints use CVars AdvancedPawn

Stat command for drawing applied forces as lines and arrows.
```
p.RacingVehicle.AdvPawn.DrawAllForces 1
```
Automatically this CVar activates p.Chaos.Debug Draw.Enabled

Stat ```DECLARE_CYCLE_STAT``` Allows you to see in PIE the number of calls and the time taken in ms. Userful for debugging and profiling.

1. For AdvancedPawn calls AdvancedTick and AdvancedNativeTick
```
stat AdvancedPawn
```
<div align="center">
    <img src="Resources/Docs/StatAdvancedPawn.png" alt="StatAdvancedPawn" width="800">
</div>

2. For AdvancedCallback calls ProcessInputs_Internal and OnPreSimulate_Internal
```
stat RacingManager
```
<div align="center">
    <img src="Resources/Docs/StatRacingManager.png" alt="StatRacingManager" width="800">
</div>

Throbleshooting and TODO
=====================
There are no faults yet...

TODO:
1. Refix UAdvancedMovementComponent.
2. Realize AddForce for UPrimitiveComponents
3. More stabilize.

Where is it used?
=====================
1. NWheelVehicle project - Author: https://boosty.to/ivan_novozhilov
<div align="center">
    <img src="Resources/Docs/RedCar.png" alt="NWRedCar" width="700">
    <img src="Resources/Docs/Car.png" alt="NWCar" width="700">
</div>

---
You can use this plugin anywhere and for free, except for those that do not comply with the ***Unreal Engine*** brand.
