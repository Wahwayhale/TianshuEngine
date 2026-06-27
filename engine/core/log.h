#pragma once

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <memory>

namespace spark {

class Log {
public:
    static void init();

    static std::shared_ptr<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
    static std::shared_ptr<spdlog::logger>& getClientLogger() { return s_clientLogger; }

private:
    static std::shared_ptr<spdlog::logger> s_coreLogger;
    static std::shared_ptr<spdlog::logger> s_clientLogger;
};

} // namespace spark

// Core log macros
#define SPARK_CORE_TRACE(...)    ::spark::Log::getCoreLogger()->trace(__VA_ARGS__)
#define SPARK_CORE_INFO(...)     ::spark::Log::getCoreLogger()->info(__VA_ARGS__)
#define SPARK_CORE_WARN(...)     ::spark::Log::getCoreLogger()->warn(__VA_ARGS__)
#define SPARK_CORE_ERROR(...)    ::spark::Log::getCoreLogger()->error(__VA_ARGS__)
#define SPARK_CORE_CRITICAL(...) ::spark::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SPARK_TRACE(...)    ::spark::Log::getClientLogger()->trace(__VA_ARGS__)
#define SPARK_INFO(...)     ::spark::Log::getClientLogger()->info(__VA_ARGS__)
#define SPARK_WARN(...)     ::spark::Log::getClientLogger()->warn(__VA_ARGS__)
#define SPARK_ERROR(...)    ::spark::Log::getClientLogger()->error(__VA_ARGS__)
#define SPARK_CRITICAL(...) ::spark::Log::getClientLogger()->critical(__VA_ARGS__)
