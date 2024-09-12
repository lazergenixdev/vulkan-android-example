#pragma once
#include <utility>

#if defined(_WIN32)
#define OS_WINDOWS 1 // Windows Operating System
#elif defined(__ANDROID__)
#define OS_ANDROID 1 // Android Operating System
#elif defined(__MACOS__) // this does not actually work btw
#define OS_MACOS 1   // MacOS  (Operating System)
#endif

namespace os
{
    void initialize();
    void shutdown();

    void show_dialog(char const* title, char const* message);
}

#if   OS_WINDOWS  // --------------------------------------------------

#define VK_USE_PLATFORM_WIN32_KHR 1
#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include <cstdio>
#include <type_traits>
#define OS_MAIN() DWORD main(void*)
#define OS_INSTANCE
#define OS_EXIT_CODE 0

template <typename...T>
static void __log(char const* format, T&&...args) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), format, std::forward<T>(args)...);
    OutputDebugStringA(buffer);
}

#define LOGIf(FMT, ...) __log("Engine [%s] " FMT "\n", __FUNCTION__, __VA_ARGS__)
#define LOGI(MSG)       __log("Engine [%s] " MSG "\n", __FUNCTION__)

namespace os
{
    using Mutex = HANDLE;
    using Thread = HANDLE;

    struct Instance {};

    using Window = HWND;
}

#define os_create_mutex(mutex)  (mutex) = CreateMutexW(nullptr, FALSE, nullptr)
#define os_destroy_mutex(mutex) CloseHandle(mutex)
#define os_lock_mutex(mutex)    WaitForSingleObject(mutex, INFINITE)
#define os_unlock_mutex(mutex)  ReleaseMutex(mutex)

#define __PRETTY_FUNCTION__ __FUNCSIG__

#elif OS_ANDROID  // --------------------------------------------------

#define VK_USE_PLATFORM_ANDROID_KHR 1
#include <android/log.h>
#include <android/native_activity.h>

#define OS_MAIN() void* android_main(void*)
#define OS_INSTANCE
#define OS_EXIT_CODE nullptr

template <typename...T>
static void __log(int level, char const* function_name, char const* format, T&&...args) {
    char buffer[512];
    snprintf(buffer, sizeof(buffer), format, std::forward<T>(args)...);
    __android_log_write(level, function_name, buffer);
}

#define LOGIf(FMT, ...) __log(ANDROID_LOG_INFO, "Engine", "[%s] " FMT, __FUNCTION__, __VA_ARGS__)
#define LOGI(MSG)       __log(ANDROID_LOG_INFO, "Engine", "[%s] " MSG, __FUNCTION__)

namespace os
{
    using Mutex = pthread_mutex_t;
    using Thread = pthread_t;

    struct Instance {};

    using Window = struct ANativeWindow*;
}

#define os_create_mutex(mutex)  pthread_mutex_init(&(mutex), nullptr)
#define os_destroy_mutex(mutex) pthread_mutex_destroy(&(mutex))
#define os_lock_mutex(mutex)    pthread_mutex_lock(&(mutex))
#define os_unlock_mutex(mutex)  pthread_mutex_unlock(&(mutex))

#endif // ------- // --------------------------------------------------
