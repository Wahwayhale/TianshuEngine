#include "vehicle.h"
#include "jolt_physics.h"
#include "core/log.h"
#include <algorithm>
#include <cmath>

namespace spark {

Vehicle::Vehicle(const VehicleDesc& desc) : m_desc(desc) {
    // 初始化车轮状态
    m_state.wheelStates.resize(desc.wheels.size());
}

Vehicle::~Vehicle() = default;

void Vehicle::update(float deltaTime) {
    updateEngine(deltaTime);
    updateTransmission(deltaTime);
    updateSteering(deltaTime);
    updateWheels(deltaTime);
    updatePhysics(deltaTime);
}

void Vehicle::setThrottle(float throttle) {
    m_throttle = std::clamp(throttle, 0.0f, 1.0f);
}

void Vehicle::setBrake(float brake) {
    m_brake = std::clamp(brake, 0.0f, 1.0f);
    m_state.isBraking = m_brake > 0.1f;
}

void Vehicle::setSteering(float steering) {
    m_steering = std::clamp(steering, -1.0f, 1.0f);
}

void Vehicle::setHandbrake(bool handbrake) {
    m_handbrake = handbrake;
    m_state.isHandbrake = handbrake;
}

void Vehicle::shiftUp() {
    if (m_state.currentGear < m_desc.gearCount) {
        m_state.currentGear++;
        SPARK_CORE_INFO("Shifted up to gear {0}", m_state.currentGear);
    }
}

void Vehicle::shiftDown() {
    if (m_state.currentGear > 1) {
        m_state.currentGear--;
        SPARK_CORE_INFO("Shifted down to gear {0}", m_state.currentGear);
    }
}

void Vehicle::updateEngine(float deltaTime) {
    // 计算引擎扭矩
    float targetRPM = m_desc.idleRPM + m_throttle * (m_desc.maxEngineRPM - m_desc.idleRPM);
    m_state.engineRPM = m_state.engineRPM + (targetRPM - m_state.engineRPM) * deltaTime * 5.0f;

    // 限制 RPM
    m_state.engineRPM = std::clamp(m_state.engineRPM, m_desc.idleRPM, m_desc.maxEngineRPM);

    // 计算引擎扭矩（简化）
    float rpmNormalized = (m_state.engineRPM - m_desc.idleRPM) / (m_desc.maxEngineRPM - m_desc.idleRPM);
    m_engineTorque = m_desc.engineForce * m_throttle * rpmNormalized;
}

void Vehicle::updateTransmission(float deltaTime) {
    if (!m_autoTransmission) return;

    // 自动换挡逻辑
    float speedKmh = m_state.speed * 3.6f;

    // 根据速度和 RPM 自动换挡
    if (m_state.engineRPM > m_desc.maxEngineRPM * 0.9f && m_state.currentGear < m_desc.gearCount) {
        shiftUp();
    } else if (m_state.engineRPM < m_desc.idleRPM * 1.5f && m_state.currentGear > 1) {
        shiftDown();
    }

    // 计算车轮扭矩
    float gearRatio = m_desc.gearRatios[m_state.currentGear - 1];
    m_wheelTorque = m_engineTorque * gearRatio * m_desc.finalDriveRatio;
}

void Vehicle::updateSteering(float deltaTime) {
    // 计算转向角度
    float targetSteering = m_steering * m_desc.maxSteeringAngle;
    m_state.steeringAngle = m_state.steeringAngle + (targetSteering - m_state.steeringAngle) * deltaTime * m_desc.steeringSpeed;

    // 更新前轮转向
    for (size_t i = 0; i < m_desc.wheels.size(); i++) {
        if (m_desc.wheels[i].isFront) {
            m_state.wheelStates[i].steeringAngle = m_state.steeringAngle;
        }
    }
}

void Vehicle::updateWheels(float deltaTime) {
    for (size_t i = 0; i < m_desc.wheels.size(); i++) {
        auto& wheel = m_desc.wheels[i];
        auto& state = m_state.wheelStates[i];

        // 更新车轮旋转
        state.rotation += state.rpm * deltaTime * 2.0f * 3.14159f / 60.0f;

        // 计算车轮 RPM（简化）
        if (m_state.speed > 0.1f) {
            state.rpm = m_state.speed * 60.0f / (2.0f * 3.14159f * wheel.radius);
        } else {
            state.rpm = 0.0f;
        }
    }
}

void Vehicle::updatePhysics(float deltaTime) {
    // 更新车辆物理
    // 简化实现：实际需要集成物理引擎

    // 计算速度
    m_state.speed = std::abs(m_state.speed);
}

// =============================================
// VehicleManager
// =============================================

VehicleManager& VehicleManager::get() {
    static VehicleManager instance;
    return instance;
}

std::shared_ptr<Vehicle> VehicleManager::createVehicle(const VehicleDesc& desc) {
    auto vehicle = std::make_shared<Vehicle>(desc);
    m_vehicles.push_back(vehicle);
    SPARK_CORE_INFO("Vehicle created.");
    return vehicle;
}

void VehicleManager::destroyVehicle(std::shared_ptr<Vehicle> vehicle) {
    auto it = std::find(m_vehicles.begin(), m_vehicles.end(), vehicle);
    if (it != m_vehicles.end()) {
        m_vehicles.erase(it);
    }
}

void VehicleManager::update(float deltaTime) {
    for (auto& vehicle : m_vehicles) {
        vehicle->update(deltaTime);
    }
}

} // namespace spark
