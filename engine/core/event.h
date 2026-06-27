#pragma once

#include <functional>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <cstdint>

namespace spark {

// Base event type
struct Event {
    virtual ~Event() = default;
    bool handled = false;
};

// Event dispatcher using type erasure
class EventDispatcher {
public:
    template<typename T>
    using CallbackFn = std::function<bool(T&)>;

    template<typename T>
    void addListener(CallbackFn<T> callback) {
        auto typeIdx = std::type_index(typeid(T));
        m_listeners[typeIdx].push_back([callback = std::move(callback)](Event& e) {
            return callback(static_cast<T&>(e));
        });
    }

    template<typename T>
    void dispatch(T& event) {
        auto typeIdx = std::type_index(typeid(T));
        if (m_listeners.find(typeIdx) != m_listeners.end()) {
            for (auto& listener : m_listeners[typeIdx]) {
                if (listener(event)) {
                    event.handled = true;
                    break;
                }
            }
        }
    }

    void clear() { m_listeners.clear(); }

private:
    std::unordered_map<std::type_index, std::vector<std::function<bool(Event&)>>> m_listeners;
};

// Window events
struct WindowResizeEvent : public Event {
    int width, height;
    WindowResizeEvent(int w, int h) : width(w), height(h) {}
};

struct WindowCloseEvent : public Event {};

// Input events
struct KeyEvent : public Event {
    int key, action;
    KeyEvent(int k, int a) : key(k), action(a) {}
};

struct MouseButtonEvent : public Event {
    int button, action;
    MouseButtonEvent(int b, int a) : button(b), action(a) {}
};

struct MouseMoveEvent : public Event {
    double x, y;
    MouseMoveEvent(double mx, double my) : x(mx), y(my) {}
};

struct MouseScrollEvent : public Event {
    double xOffset, yOffset;
    MouseScrollEvent(double x, double y) : xOffset(x), yOffset(y) {}
};

} // namespace spark
