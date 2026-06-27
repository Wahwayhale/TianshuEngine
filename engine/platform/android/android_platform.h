#pragma once

#ifdef PLATFORM_ANDROID

#include <android/native_activity.h>
#include <android/native_window.h>
#include <android/log.h>
#include <android/sensor.h>
#include <jni.h>
#include <string>

namespace spark {

// Android 日志宏
#define ANDROID_LOG_INFO(tag, ...) __android_log_print(ANDROID_LOG_INFO, tag, __VA_ARGS__)
#define ANDROID_LOG_WARN(tag, ...) __android_log_print(ANDROID_LOG_WARN, tag, __VA_ARGS__)
#define ANDROID_LOG_ERROR(tag, ...) __android_log_print(ANDROID_LOG_ERROR, tag, __VA_ARGS__)

// Android 平台类
class AndroidPlatform {
public:
    static AndroidPlatform& get();

    // 初始化
    bool initialize(ANativeActivity* activity, void* savedState, size_t savedStateSize);
    void shutdown();

    // 窗口管理
    ANativeWindow* getWindow() const { return m_window; }
    void setWindow(ANativeWindow* window);
    int getWindowWidth() const;
    int getWindowHeight() const;

    // 输入处理
    void handleInput(AInputEvent* event);
    void handleMotion(AInputEvent* event);

    // 传感器
    void enableAccelerometer();
    void disableAccelerometer();
    void enableGyroscope();
    void disableGyroscope();

    // 生命周期
    void onResume();
    void onPause();
    void onDestroy();

    // 文件系统
    std::string getInternalStoragePath() const;
    std::string getExternalStoragePath() const;
    std::string getAssetsPath() const;

    // 资产加载
    bool loadAsset(const std::string& path, std::vector<char>& data);

    // 显示信息
    float getDensity() const;
    int getDPI() const;

private:
    AndroidPlatform() = default;

    ANativeActivity* m_activity = nullptr;
    ANativeWindow* m_window = nullptr;
    ASensorManager* m_sensorManager = nullptr;
    ASensor* m_accelerometer = nullptr;
    ASensor* m_gyroscope = nullptr;
    ASensorEventQueue* m_sensorEventQueue = nullptr;

    std::string m_internalStoragePath;
    std::string m_externalStoragePath;
};

// JNI 工具函数
namespace jni {
    JNIEnv* getEnv();
    jclass findClass(const char* name);
    jmethodID getMethodID(jclass clazz, const char* name, const char* sig);
    jmethodID getStaticMethodID(jclass clazz, const char* name, const char* sig);
}

} // namespace spark

#endif // PLATFORM_ANDROID
