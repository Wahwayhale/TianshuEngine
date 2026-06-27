#pragma once

#include <functional>

namespace spark {

class ToolbarPanel {
public:
    ToolbarPanel();
    ~ToolbarPanel();

    void render();

    // 回调设置
    using PlayCallback = std::function<void()>;
    using PauseCallback = std::function<void()>;
    using StopCallback = std::function<void()>;

    void setPlayCallback(PlayCallback callback) { m_playCallback = callback; }
    void setPauseCallback(PauseCallback callback) { m_pauseCallback = callback; }
    void setStopCallback(StopCallback callback) { m_stopCallback = callback; }

private:
    void renderSceneControls();
    void renderTransformTools();
    void renderViewOptions();
    void renderQuickActions();

    PlayCallback m_playCallback;
    PauseCallback m_pauseCallback;
    StopCallback m_stopCallback;

    // 状态
    bool m_isPlaying = false;
    bool m_isPaused = false;
    int m_currentTool = 0;  // 0=Move, 1=Rotate, 2=Scale
};

} // namespace spark
