#pragma once

#ifdef PLATFORM_WEB

#include <emscripten.h>
#include <emscripten/html5.h>
#include <emscripten/val.h>
#include <string>

namespace spark {

// Web 平台类
class WebPlatform {
public:
    static WebPlatform& get();

    // 初始化
    bool initialize();
    void shutdown();

    // 窗口管理
    int getCanvasWidth() const;
    int getCanvasHeight() const;
    void setCanvasSize(int width, int height);

    // 输入处理
    static EM_BOOL onKeyDown(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);
    static EM_BOOL onKeyUp(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData);
    static EM_BOOL onMouseMove(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
    static EM_BOOL onMouseDown(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
    static EM_BOOL onMouseUp(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData);
    static EM_BOOL onWheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData);
    static EM_BOOL onTouchStart(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData);
    static EM_BOOL onTouchMove(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData);
    static EM_BOOL onTouchEnd(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData);
    static EM_BOOL onResize(int eventType, const EmscriptenUiEvent* uiEvent, void* userData);

    // 全屏
    void requestFullscreen();
    void exitFullscreen();
    bool isFullscreen() const;

    // 文件系统
    void mountFileSystem();
    void syncFileSystem();

    // 资源加载
    bool loadFile(const std::string& path, std::vector<char>& data);
    void loadFileAsync(const std::string& path, std::function<void(std::vector<char>)> callback);

    // 浏览器 API
    void openURL(const std::string& url);
    std::string getClipboard();
    void setClipboard(const std::string& text);

    // 性能
    double getTime() const;

private:
    WebPlatform() = default;

    std::string m_canvasId = "canvas";
    bool m_initialized = false;
};

} // namespace spark

#endif // PLATFORM_WEB
