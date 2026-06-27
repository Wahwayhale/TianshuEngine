#include "android_platform.h"

#ifdef PLATFORM_ANDROID

#include <android/asset_manager.h>
#include <android/configuration.h>
#include <fstream>

namespace spark {

AndroidPlatform& AndroidPlatform::get() {
    static AndroidPlatform instance;
    return instance;
}

bool AndroidPlatform::initialize(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    m_activity = activity;

    // 获取内部存储路径
    m_internalStoragePath = activity->internalDataPath;
    m_externalStoragePath = activity->externalDataPath ? activity->externalDataPath : "";

    // 初始化传感器管理器
    m_sensorManager = ASensorManager_getInstance(nullptr);
    if (m_sensorManager) {
        m_accelerometer = ASensorManager_getDefaultSensor(m_sensorManager, ASENSOR_TYPE_ACCELEROMETER);
        m_gyroscope = ASensorManager_getDefaultSensor(m_sensorManager, ASENSOR_TYPE_GYROSCOPE);
    }

    ANDROID_LOG_INFO("TianshuEngine", "Android platform initialized");
    return true;
}

void AndroidPlatform::shutdown() {
    disableAccelerometer();
    disableGyroscope();

    if (m_sensorEventQueue) {
        ASensorManager_destroyEventQueue(m_sensorManager, m_sensorEventQueue);
        m_sensorEventQueue = nullptr;
    }

    ANDROID_LOG_INFO("TianshuEngine", "Android platform shutdown");
}

void AndroidPlatform::setWindow(ANativeWindow* window) {
    m_window = window;
    if (window) {
        ANDROID_LOG_INFO("TianshuEngine", "Window set: %dx%d",
                         ANativeWindow_getWidth(window),
                         ANativeWindow_getHeight(window));
    }
}

int AndroidPlatform::getWindowWidth() const {
    return m_window ? ANativeWindow_getWidth(m_window) : 0;
}

int AndroidPlatform::getWindowHeight() const {
    return m_window ? ANativeWindow_getHeight(m_window) : 0;
}

void AndroidPlatform::handleInput(AInputEvent* event) {
    if (!event) return;

    int type = AInputEvent_getType(event);
    if (type == AINPUT_EVENT_TYPE_MOTION) {
        handleMotion(event);
    }
}

void AndroidPlatform::handleMotion(AInputEvent* event) {
    int action = AMotionEvent_getAction(event);
    int actionMasked = action & AMOTION_EVENT_ACTION_MASK;

    float x = AMotionEvent_getX(event, 0);
    float y = AMotionEvent_getY(event, 0);

    switch (actionMasked) {
        case AMOTION_EVENT_ACTION_DOWN:
            ANDROID_LOG_INFO("TianshuEngine", "Touch down: (%.1f, %.1f)", x, y);
            break;
        case AMOTION_EVENT_ACTION_UP:
            ANDROID_LOG_INFO("TianshuEngine", "Touch up: (%.1f, %.1f)", x, y);
            break;
        case AMOTION_EVENT_ACTION_MOVE:
            // 处理触摸移动
            break;
    }
}

void AndroidPlatform::enableAccelerometer() {
    if (m_sensorManager && m_accelerometer && !m_sensorEventQueue) {
        m_sensorEventQueue = ASensorManager_createEventQueue(m_sensorManager, nullptr, 0, nullptr, nullptr);
        ASensorEventQueue_enableSensor(m_sensorEventQueue, m_accelerometer);
        ASensorEventQueue_setEventRate(m_sensorEventQueue, m_accelerometer, 1000000 / 60);  // 60Hz
    }
}

void AndroidPlatform::disableAccelerometer() {
    if (m_sensorEventQueue && m_accelerometer) {
        ASensorEventQueue_disableSensor(m_sensorEventQueue, m_accelerometer);
    }
}

void AndroidPlatform::enableGyroscope() {
    if (m_sensorManager && m_gyroscope && m_sensorEventQueue) {
        ASensorEventQueue_enableSensor(m_sensorEventQueue, m_gyroscope);
        ASensorEventQueue_setEventRate(m_sensorEventQueue, m_gyroscope, 1000000 / 60);
    }
}

void AndroidPlatform::disableGyroscope() {
    if (m_sensorEventQueue && m_gyroscope) {
        ASensorEventQueue_disableSensor(m_sensorEventQueue, m_gyroscope);
    }
}

void AndroidPlatform::onResume() {
    if (m_sensorEventQueue && m_accelerometer) {
        ASensorEventQueue_enableSensor(m_sensorEventQueue, m_accelerometer);
    }
    ANDROID_LOG_INFO("TianshuEngine", "Resumed");
}

void AndroidPlatform::onPause() {
    if (m_sensorEventQueue && m_accelerometer) {
        ASensorEventQueue_disableSensor(m_sensorEventQueue, m_accelerometer);
    }
    ANDROID_LOG_INFO("TianshuEngine", "Paused");
}

void AndroidPlatform::onDestroy() {
    shutdown();
    ANDROID_LOG_INFO("TianshuEngine", "Destroyed");
}

std::string AndroidPlatform::getInternalStoragePath() const {
    return m_internalStoragePath;
}

std::string AndroidPlatform::getExternalStoragePath() const {
    return m_externalStoragePath;
}

std::string AndroidPlatform::getAssetsPath() const {
    return "";  // Android assets are accessed via AAssetManager
}

bool AndroidPlatform::loadAsset(const std::string& path, std::vector<char>& data) {
    AAssetManager* assetManager = m_activity->assetManager;
    if (!assetManager) return false;

    AAsset* asset = AAssetManager_open(assetManager, path.c_str(), AASSET_MODE_BUFFER);
    if (!asset) return false;

    size_t size = AAsset_getLength(asset);
    data.resize(size);
    AAsset_read(asset, data.data(), size);
    AAsset_close(asset);

    return true;
}

float AndroidPlatform::getDensity() const {
    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, m_activity->assetManager);

    int density = AConfiguration_getDensity(config);
    AConfiguration_delete(config);

    return density > 0 ? density / 160.0f : 1.0f;
}

int AndroidPlatform::getDPI() const {
    AConfiguration* config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, m_activity->assetManager);

    int density = AConfiguration_getDensity(config);
    AConfiguration_delete(config);

    return density > 0 ? density : 160;
}

// =============================================
// JNI 工具函数
// =============================================

namespace jni {

JNIEnv* getEnv() {
    JNIEnv* env = nullptr;
    // 获取 JNI 环境
    return env;
}

jclass findClass(const char* name) {
    JNIEnv* env = getEnv();
    return env ? env->FindClass(name) : nullptr;
}

jmethodID getMethodID(jclass clazz, const char* name, const char* sig) {
    JNIEnv* env = getEnv();
    return env ? env->GetMethodID(clazz, name, sig) : nullptr;
}

jmethodID getStaticMethodID(jclass clazz, const char* name, const char* sig) {
    JNIEnv* env = getEnv();
    return env ? env->GetStaticMethodID(clazz, name, sig) : nullptr;
}

} // namespace jni

} // namespace spark

#endif // PLATFORM_ANDROID
