Advanced Physics Tick
=====================

<div align="center">

![Alt text](Resources\Docs\Banner.png)

</div>

Advanced Physics Tick is a plugin for Unreal Engine 5 designed to work with asynchronous Chaos physics. It is specifically focused on vehicle simulation and includes a single UStaticMesh (VehicleMesh) that provides more robust and improved control over the physics model. This plugin is not a general-purpose plugin, but is designed with an emphasis on accurate and flexible motion simulation for Vehicles.

Setup
=====================

1. Clone or download the repository from GitHub:
```
git clone https://github.com/your-repo/AdvancedPhysicsTick.git
```
2. Place the plugin folder in the plugins directory of your Unreal Engine 5 project.
3. Run Unreal Engine 5 project.

Documentation
=====================

### 1. AddForce, AddForceAtLocation and etc...
Use these functions to interact with VehicleMesh in AAdvancedPawn:
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
	> NOTE. THREADSAFE! CALL THIS FUNCTIONS ONLY AdvancedTick in BP or PhysicsTick in C++!

### 2. GetLinearVelocityAtPoint
```
UFUNCTION(BlueprintCallable, Category = "AdvancedPawn|Physics")
FVector GetLinearVelocityAtPoint(const UPrimitiveComponent* TargetComponent, const FVector& InPoint);
```

	> NOTE. THREADSAFE! CALL THIS FUNCTION ONLY AdvancedTick in BP or PhysicsTick in C++!