#pragma once

#include "math/math_types.h"
#include <vector>
#include <memory>
#include <cstdint>

namespace spark {

using PhysicsBodyHandle = uint32_t;

// 车轮描述
struct WheelDesc {
    float radius = 0.3f;
    float width = 0.2f;
    float mass = 10.0f;

    // 悬挂
    float suspensionRestLength = 0.3f;
    float suspensionStiffness = 20.0f;
    float suspensionDamping = 2.0f;
    float suspensionCompression = 4.0f;
    float maxSuspensionTravel = 0.5f;

    // 轮胎
    float tireFriction = 1.0f;
    float rollInfluence = 0.1f;

    // 位置
    Vec3 connectionPoint = Vec3(0.0f);
    bool isFront = false;
};

// 车辆描述
struct VehicleDesc {
    PhysicsBodyHandle chassisBody = 0;

    // 底盘参数
    float chassisMass = 800.0f;
    Vec3 chassisSize = Vec3(2.0f, 0.5f, 4.0f);

    // 引擎
    float engineForce = 2000.0f;
    float maxEngineRPM = 6000.0f;
    float idleRPM = 800.0f;

    // 变速箱
    int gearCount = 5;
    std::vector<float> gearRatios = {3.5f, 2.5f, 1.8f, 1.3f, 1.0f};
    float reverseRatio = 3.0f;
    float finalDriveRatio = 3.5f;

    // 转向
    float maxSteeringAngle = 30.0f;  // 度
    float steeringSpeed = 2.0f;

    // 刹车
    float brakeForce = 500.0f;
    float handbrakeForce = 1000.0f;

    // 车轮
    std::vector<WheelDesc> wheels;
};

// 车轮状态
struct WheelState {
    float rotation = 0.0f;
    float steeringAngle = 0.0f;
    float rpm = 0.0f;
    bool onGround = false;
    float suspensionLength = 0.0f;
};

// 车辆状态
struct VehicleState {
    float speed = 0.0f;           // km/h
    float engineRPM = 0.0f;
    int currentGear = 1;
    float steeringAngle = 0.0f;
    bool isBraking = false;
    bool isHandbrake = false;
    std::vector<WheelState> wheelStates;
};

// 车辆物理
class Vehicle {
public:
    Vehicle(const VehicleDesc& desc);
    ~Vehicle();

    // 更新
    void update(float deltaTime);

    // 控制
    void setThrottle(float throttle);    // 0.0 - 1.0
    void setBrake(float brake);          // 0.0 - 1.0
    void setSteering(float steering);    // -1.0 - 1.0
    void setHandbrake(bool handbrake);

    // 变速箱
    void shiftUp();
    void shiftDown();
    void setAutoTransmission(bool autoTrans) { m_autoTransmission = autoTrans; }

    // 状态
    const VehicleState& getState() const { return m_state; }
    float getSpeed() const { return m_state.speed; }
    float getEngineRPM() const { return m_state.engineRPM; }
    int getCurrentGear() const { return m_state.currentGear; }

    // 物理体
    PhysicsBodyHandle getChassisBody() const { return m_desc.chassisBody; }

private:
    void updateEngine(float deltaTime);
    void updateTransmission(float deltaTime);
    void updateSteering(float deltaTime);
    void updateWheels(float deltaTime);
    void updatePhysics(float deltaTime);

    VehicleDesc m_desc;
    VehicleState m_state;

    // 控制输入
    float m_throttle = 0.0f;
    float m_brake = 0.0f;
    float m_steering = 0.0f;
    bool m_handbrake = false;
    bool m_autoTransmission = true;

    // 引擎状态
    float m_engineTorque = 0.0f;
    float m_wheelTorque = 0.0f;
};

// 车辆管理器
class VehicleManager {
public:
    static VehicleManager& get();

    // 创建车辆
    std::shared_ptr<Vehicle> createVehicle(const VehicleDesc& desc);

    // 销毁车辆
    void destroyVehicle(std::shared_ptr<Vehicle> vehicle);

    // 更新所有车辆
    void update(float deltaTime);

    // 获取车辆数量
    int getVehicleCount() const { return static_cast<int>(m_vehicles.size()); }

private:
    VehicleManager() = default;

    std::vector<std::shared_ptr<Vehicle>> m_vehicles;
};

} // namespace spark
