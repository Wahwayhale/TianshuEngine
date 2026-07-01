#include "platform.h"
#include "core/log.h"

// 平台特定头文件
#ifdef PLATFORM_WINDOWS
    #include <windows.h>
    #include <shlobj.h>
    #pragma comment(lib, "Shell32.lib")
#elif defined(PLATFORM_LINUX)
    #include <unistd.h>
    #include <sys/stat.h>
    #include <dlfcn.h>
    #include <fstream>
#elif defined(PLATFORM_MACOS)
    #include <unistd.h>
    #include <sys/stat.h>
    #include <dlfcn.h>
    #include <mach/mach.h>
#endif

namespace spark {

Platform& Platform::get() {
    static Platform instance;
    return instance;
}

bool Platform::initialize() {
    if (m_initialized) return true;

    m_initialized = true;
    SPARK_CORE_INFO("Platform initialized: {0}", getTypeName());
    return true;
}

void Platform::shutdown() {
    if (!m_initialized) return;
    m_initialized = false;
    SPARK_CORE_INFO("Platform shutdown.");
}

PlatformType Platform::getType() const {
#ifdef PLATFORM_WINDOWS
    return PlatformType::Windows;
#elif defined(PLATFORM_LINUX)
    return PlatformType::Linux;
#elif defined(PLATFORM_MACOS)
    return PlatformType::macOS;
#else
    return PlatformType::Unknown;
#endif
}

std::string Platform::getTypeName() const {
    switch (getType()) {
        case PlatformType::Windows: return "Windows";
        case PlatformType::Linux: return "Linux";
        case PlatformType::macOS: return "macOS";
        case PlatformType::Android: return "Android";
        case PlatformType::iOS: return "iOS";
        default: return "Unknown";
    }
}

std::string Platform::getOSVersion() const {
#ifdef PLATFORM_WINDOWS
    OSVERSIONINFOEXA osvi;
    ZeroMemory(&osvi, sizeof(OSVERSIONINFOEXA));
    osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXA);
    // 注意：GetVersionEx 已弃用，需要使用其他方法
    return "Windows 10+";
#elif defined(PLATFORM_LINUX)
    std::ifstream file("/etc/os-release");
    std::string line;
    while (std::getline(file, line)) {
        if (line.find("PRETTY_NAME=") != std::string::npos) {
            return line.substr(13);
        }
    }
    return "Linux";
#elif defined(PLATFORM_MACOS)
    return "macOS";
#else
    return "Unknown";
#endif
}

std::string Platform::getCPUInfo() const {
#ifdef PLATFORM_WINDOWS
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return "CPU Cores: " + std::to_string(sysInfo.dwNumberOfProcessors);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    long cores = sysconf(_SC_NPROCESSORS_ONLN);
    return "CPU Cores: " + std::to_string(cores);
#else
    return "Unknown CPU";
#endif
}

size_t Platform::getTotalMemory() const {
#ifdef PLATFORM_WINDOWS
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullTotalPhys;
#elif defined(PLATFORM_LINUX)
    long pages = sysconf(_SC_PHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return pages * page_size;
#elif defined(PLATFORM_MACOS)
    // macOS 实现
    return 0;
#else
    return 0;
#endif
}

size_t Platform::getAvailableMemory() const {
#ifdef PLATFORM_WINDOWS
    MEMORYSTATUSEX memInfo;
    memInfo.dwLength = sizeof(MEMORYSTATUSEX);
    GlobalMemoryStatusEx(&memInfo);
    return memInfo.ullAvailPhys;
#elif defined(PLATFORM_LINUX)
    long avail_pages = sysconf(_SC_AVPHYS_PAGES);
    long page_size = sysconf(_SC_PAGE_SIZE);
    return avail_pages * page_size;
#else
    return 0;
#endif
}

std::string Platform::getExecutablePath() const {
#ifdef PLATFORM_WINDOWS
    char path[MAX_PATH];
    GetModuleFileNameA(nullptr, path, MAX_PATH);
    return std::string(path);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    char path[1024];
    ssize_t count = readlink("/proc/self/exe", path, sizeof(path));
    if (count != -1) {
        return std::string(path, count);
    }
    return "";
#else
    return "";
#endif
}

std::string Platform::getWorkingDirectory() const {
#ifdef PLATFORM_WINDOWS
    char path[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, path);
    return std::string(path);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    char path[1024];
    if (getcwd(path, sizeof(path))) {
        return std::string(path);
    }
    return "";
#else
    return "";
#endif
}

std::string Platform::getUserDataPath() const {
#ifdef PLATFORM_WINDOWS
    char path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(nullptr, CSIDL_APPDATA, nullptr, 0, path))) {
        return std::string(path) + "\\SparkEngine";
    }
    return "";
#elif defined(PLATFORM_LINUX)
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/.sparkengine";
    }
    return "";
#elif defined(PLATFORM_MACOS)
    const char* home = getenv("HOME");
    if (home) {
        return std::string(home) + "/Library/Application Support/SparkEngine";
    }
    return "";
#else
    return "";
#endif
}

std::string Platform::getTempPath() const {
#ifdef PLATFORM_WINDOWS
    char path[MAX_PATH];
    GetTempPathA(MAX_PATH, path);
    return std::string(path);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    return "/tmp";
#else
    return "";
#endif
}

bool Platform::fileExists(const std::string& path) const {
#ifdef PLATFORM_WINDOWS
    DWORD attrib = GetFileAttributesA(path.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && !(attrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISREG(buffer.st_mode));
#else
    return false;
#endif
}

bool Platform::directoryExists(const std::string& path) const {
#ifdef PLATFORM_WINDOWS
    DWORD attrib = GetFileAttributesA(path.c_str());
    return (attrib != INVALID_FILE_ATTRIBUTES && (attrib & FILE_ATTRIBUTE_DIRECTORY));
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0 && S_ISDIR(buffer.st_mode));
#else
    return false;
#endif
}

bool Platform::createDirectory(const std::string& path) const {
#ifdef PLATFORM_WINDOWS
    return CreateDirectoryA(path.c_str(), nullptr) != 0;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    return mkdir(path.c_str(), 0755) == 0;
#else
    return false;
#endif
}

bool Platform::deleteFile(const std::string& path) const {
#ifdef PLATFORM_WINDOWS
    return DeleteFileA(path.c_str()) != 0;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    return unlink(path.c_str()) == 0;
#else
    return false;
#endif
}

void* Platform::loadLibrary(const std::string& path) {
#ifdef PLATFORM_WINDOWS
    return LoadLibraryA(path.c_str());
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    return dlopen(path.c_str(), RTLD_LAZY);
#else
    return nullptr;
#endif
}

void* Platform::getSymbol(void* library, const std::string& name) {
#ifdef PLATFORM_WINDOWS
    return (void*)GetProcAddress(static_cast<HMODULE>(library), name.c_str());
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    return dlsym(library, name.c_str());
#else
    return nullptr;
#endif
}

void Platform::freeLibrary(void* library) {
#ifdef PLATFORM_WINDOWS
    FreeLibrary(static_cast<HMODULE>(library));
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    dlclose(library);
#endif
}

double Platform::getTime() const {
#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return static_cast<double>(counter.QuadPart) / freq.QuadPart;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec + ts.tv_nsec / 1e9;
#else
    return 0.0;
#endif
}

uint64_t Platform::getTicks() const {
#ifdef PLATFORM_WINDOWS
    LARGE_INTEGER counter;
    QueryPerformanceCounter(&counter);
    return counter.QuadPart;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
#else
    return 0;
#endif
}

uint32_t Platform::getThreadCount() const {
#ifdef PLATFORM_WINDOWS
    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    return sysInfo.dwNumberOfProcessors;
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    return sysconf(_SC_NPROCESSORS_ONLN);
#else
    return 1;
#endif
}

void Platform::sleep(uint32_t milliseconds) {
#ifdef PLATFORM_WINDOWS
    Sleep(milliseconds);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    usleep(milliseconds * 1000);
#endif
}

void Platform::showMessage(const std::string& title, const std::string& message, MessageType type) {
#ifdef PLATFORM_WINDOWS
    UINT flags = MB_OK;
    switch (type) {
        case MessageType::Info: flags |= MB_ICONINFORMATION; break;
        case MessageType::Warning: flags |= MB_ICONWARNING; break;
        case MessageType::Error: flags |= MB_ICONERROR; break;
    }
    MessageBoxA(nullptr, message.c_str(), title.c_str(), flags);
#elif defined(PLATFORM_LINUX) || defined(PLATFORM_MACOS)
    // Linux/macOS 可以使用 zenity 或 osascript
    SPARK_CORE_INFO("[{0}] {1}: {2}", getTypeName(), title, message);
#endif
}

std::string Platform::getClipboard() const {
#ifdef PLATFORM_WINDOWS
    if (!OpenClipboard(nullptr)) return "";
    HANDLE hData = GetClipboardData(CF_TEXT);
    if (hData == nullptr) {
        CloseClipboard();
        return "";
    }
    char* text = static_cast<char*>(GlobalLock(hData));
    std::string result(text);
    GlobalUnlock(hData);
    CloseClipboard();
    return result;
#else
    return "";
#endif
}

void Platform::setClipboard(const std::string& text) {
#ifdef PLATFORM_WINDOWS
    if (!OpenClipboard(nullptr)) return;
    EmptyClipboard();
    HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, text.size() + 1);
    char* pMem = static_cast<char*>(GlobalLock(hMem));
    memcpy(pMem, text.c_str(), text.size() + 1);
    GlobalUnlock(hMem);
    SetClipboardData(CF_TEXT, hMem);
    CloseClipboard();
#endif
}

bool Platform::isNetworkAvailable() const {
    // 简化实现
    return true;
}

} // namespace spark
