#include "web_platform.h"

#ifdef PLATFORM_WEB

#include <emscripten/fetch.h>
#include <cstring>

namespace spark {

WebPlatform& WebPlatform::get() {
    static WebPlatform instance;
    return instance;
}

bool WebPlatform::initialize() {
    if (m_initialized) return true;

    // 注册输入回调
    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, onKeyDown);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_DOCUMENT, this, true, onKeyUp);
    emscripten_set_mousedown_callback("#canvas", this, true, onMouseDown);
    emscripten_set_mouseup_callback("#canvas", this, true, onMouseUp);
    emscripten_set_mousemove_callback("#canvas", this, true, onMouseMove);
    emscripten_set_wheel_callback("#canvas", this, true, onWheel);
    emscripten_set_touchstart_callback("#canvas", this, true, onTouchStart);
    emscripten_set_touchmove_callback("#canvas", this, true, onTouchMove);
    emscripten_set_touchend_callback("#canvas", this, true, onTouchEnd);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, true, onResize);

    // 挂载文件系统
    mountFileSystem();

    m_initialized = true;
    return true;
}

void WebPlatform::shutdown() {
    if (!m_initialized) return;
    syncFileSystem();
    m_initialized = false;
}

int WebPlatform::getCanvasWidth() const {
    double width;
    emscripten_get_element_client_size("#canvas", &width, nullptr);
    return static_cast<int>(width);
}

int WebPlatform::getCanvasHeight() const {
    double height;
    emscripten_get_element_client_size("#canvas", nullptr, &height);
    return static_cast<int>(height);
}

void WebPlatform::setCanvasSize(int width, int height) {
    char js[256];
    snprintf(js, sizeof(js),
        "document.getElementById('canvas').width = %d; document.getElementById('canvas').height = %d;",
        width, height);
    emscripten_run_script(js);
}

EM_BOOL WebPlatform::onKeyDown(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {
    // 处理键盘按下
    return EM_TRUE;
}

EM_BOOL WebPlatform::onKeyUp(int eventType, const EmscriptenKeyboardEvent* keyEvent, void* userData) {
    // 处理键盘释放
    return EM_TRUE;
}

EM_BOOL WebPlatform::onMouseMove(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // 处理鼠标移动
    return EM_TRUE;
}

EM_BOOL WebPlatform::onMouseDown(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // 处理鼠标按下
    return EM_TRUE;
}

EM_BOOL WebPlatform::onMouseUp(int eventType, const EmscriptenMouseEvent* mouseEvent, void* userData) {
    // 处理鼠标释放
    return EM_TRUE;
}

EM_BOOL WebPlatform::onWheel(int eventType, const EmscriptenWheelEvent* wheelEvent, void* userData) {
    // 处理滚轮
    return EM_TRUE;
}

EM_BOOL WebPlatform::onTouchStart(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData) {
    // 处理触摸开始
    return EM_TRUE;
}

EM_BOOL WebPlatform::onTouchMove(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData) {
    // 处理触摸移动
    return EM_TRUE;
}

EM_BOOL WebPlatform::onTouchEnd(int eventType, const EmscriptenTouchEvent* touchEvent, void* userData) {
    // 处理触摸结束
    return EM_TRUE;
}

EM_BOOL WebPlatform::onResize(int eventType, const EmscriptenUiEvent* uiEvent, void* userData) {
    // 处理窗口大小改变
    return EM_TRUE;
}

void WebPlatform::requestFullscreen() {
    emscripten_request_fullscreen("#canvas", true);
}

void WebPlatform::exitFullscreen() {
    emscripten_exit_fullscreen();
}

bool WebPlatform::isFullscreen() const {
    return emscripten_fullscreen_element() != nullptr;
}

void WebPlatform::mountFileSystem() {
    // 挂载 IDBFS 用于持久化存储
    EM_ASM(
        FS.mkdir('/working');
        FS.mount(IDBFS, {}, '/working');
        FS.syncfs(true, function(err) {
            if (err) console.error('FS sync error:', err);
        });
    );
}

void WebPlatform::syncFileSystem() {
    EM_ASM(
        FS.syncfs(false, function(err) {
            if (err) console.error('FS sync error:', err);
        });
    );
}

bool WebPlatform::loadFile(const std::string& path, std::vector<char>& data) {
    // 尝试从文件系统加载
    FILE* file = fopen(path.c_str(), "rb");
    if (!file) {
        // 尝试从嵌入式资源加载
        std::string embeddedPath = "/data/" + path;
        file = fopen(embeddedPath.c_str(), "rb");
    }

    if (!file) return false;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);

    data.resize(size);
    fread(data.data(), 1, size, file);
    fclose(file);

    return true;
}

void WebPlatform::loadFileAsync(const std::string& path, std::function<void(std::vector<char>)> callback) {
    // 使用 Emscripten Fetch API 异步加载
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;

    // 注意：实际实现需要处理回调
    // 这里是简化版本
}

void WebPlatform::openURL(const std::string& url) {
    char js[512];
    snprintf(js, sizeof(js), "window.open('%s', '_blank');", url.c_str());
    emscripten_run_script(js);
}

std::string WebPlatform::getClipboard() {
    // 使用 JS API 获取剪贴板
    char* text = (char*)EM_ASM_PTR({
        var text = navigator.clipboard.readText();
        var lengthBytes = lengthBytesUTF8(text) + 1;
        var stringOnWasmHeap = _malloc(lengthBytes);
        stringToUTF8(text, stringOnWasmHeap, lengthBytes);
        return stringOnWasmHeap;
    });

    std::string result = text ? text : "";
    free(text);
    return result;
}

void WebPlatform::setClipboard(const std::string& text) {
    char js[1024];
    snprintf(js, sizeof(js), "navigator.clipboard.writeText('%s');", text.c_str());
    emscripten_run_script(js);
}

double WebPlatform::getTime() const {
    return emscripten_get_now() / 1000.0;
}

} // namespace spark

#endif // PLATFORM_WEB
