# 天枢引擎 - 物理系统

## 概述

天枢引擎包含自研物理系统，支持刚体模拟、碰撞检测、关节约束等功能。

## 物理世界

### 创建物理世界

```cpp
auto physicsWorld = std::make_unique<PhysicsWorld>();
physicsWorld->initialize();
physicsWorld->setGravity(Vec3(0.0f, -9.81f, 0.0f));
```

### 更新物理

```cpp
physicsWorld->update(deltaTime);
```

## 刚体

### 创建刚体

```cpp
PhysicsBodyDesc desc;
desc.type = PhysicsBodyType::Dynamic;
desc.shapeType = PhysicsShapeType::Box;
desc.halfExtents = Vec3(0.5f);
desc.mass = 1.0f;
desc.restitution = 0.3f;
desc.friction = 0.5f;

auto handle = physicsWorld->createBody(desc);
```

### 物理体类型

| 类型 | 说明 |
|------|------|
| Static | 静态物体，不受力影响 |
| Dynamic | 动态物体，受力影响 |
| Kinematic | 运动学物体，不受力但可碰撞 |

### 碰撞形状

| 形状 | 参数 |
|------|------|
| Box | halfExtents (Vec3) |
| Sphere | radius (float) |
| Capsule | radius, height (float) |

### 施加力

```cpp
physicsWorld->applyForce(handle, Vec3(10.0f, 0.0f, 0.0f));
physicsWorld->applyImpulse(handle, Vec3(0.0f, 5.0f, 0.0f));
```

## 碰撞检测

### 射线检测

```cpp
RaycastHit hit;
if (physicsWorld->raycast(origin, direction, maxDistance, hit)) {
    // hit.point - 碰撞点
    // hit.normal - 碰撞法线
    // hit.distance - 距离
}
```

### 碰撞事件

```cpp
auto events = physicsWorld->getCollisionEvents();
for (const auto& event : events) {
    // event.bodyA - 物体A
    // event.bodyB - 物体B
    // event.contactPoint - 接触点
}
```

## 关节约束

### 创建关节

```cpp
JointDesc desc;
desc.type = JointType::Hinge;
desc.bodyA = bodyA;
desc.bodyB = bodyB;
desc.anchorA = Vec3(0.0f, 0.5f, 0.0f);
desc.anchorB = Vec3(0.0f, -0.5f, 0.0f);
desc.lowerLimit = -45.0f;
desc.upperLimit = 45.0f;

auto joint = JointManager::get().createJoint(desc);
```

### 关节类型

| 类型 | 说明 |
|------|------|
| Fixed | 固定关节 |
| Hinge | 铰链关节 |
| Slider | 滑块关节 |
| Spherical | 球窝关节 |
| Spring | 弹簧关节 |
| Distance | 距离约束 |

## 车辆物理

### 创建车辆

```cpp
VehicleDesc desc;
desc.chassisMass = 800.0f;
desc.engineForce = 2000.0f;
desc.maxSteeringAngle = 30.0f;

// 添加车轮
WheelDesc wheel;
wheel.radius = 0.3f;
wheel.isFront = true;
desc.wheels.push_back(wheel);

auto vehicle = VehicleManager::get().createVehicle(desc);
```

### 控制车辆

```cpp
vehicle->setThrottle(0.8f);
vehicle->setSteering(0.3f);
vehicle->setBrake(0.5f);
```

## 布料模拟

### 创建布料

```cpp
ClothSettings settings;
settings.gridSizeX = 20;
settings.gridSizeY = 20;
settings.spacing = 0.1f;
settings.stiffness = 100.0f;

ClothSimulation cloth(settings);
cloth.pinTopRow();
cloth.setWind(Vec3(1.0f, 0.0f, 0.5f));
```

### 更新布料

```cpp
cloth.update(deltaTime);

auto positions = cloth.getPositions();
auto normals = cloth.getNormals();
auto indices = cloth.getIndices();
```

## 破碎系统

### 破碎物体

```cpp
DestructionSettings settings;
settings.mode = DestructionMode::Voronoi;
settings.fragmentCount = 10;
settings.impactForce = 500.0f;

auto fragments = destructionSystem->destroyObject(body, impactPoint, impactDir, settings);
```

### 破碎模式

| 模式 | 说明 |
|------|------|
| Voronoi | Voronoi 破碎 |
| Radial | 径向破碎 |
| Planar | 平面破碎 |
| Shatter | 粉碎 |

## 调试绘制

```cpp
DebugDraw::get().drawBox(center, halfExtents, rotation, DebugColor::Yellow());
DebugDraw::get().drawSphere(center, radius, DebugColor::Red());
DebugDraw::get().drawRay(origin, direction, length, DebugColor::Green());
```
