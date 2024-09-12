#pragma once

#if   defined _WIN32
#define OS_WINDOWS 1
#elif defined __ANDROID__
#define OS_ANDROID 1
#endif


#if   OS_WINDOWS  // --------------------------------------------------

#include <Windows.h>
#include <stdio.h>
#define OS_MAIN() int APIENTRY WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
#define OS_INSTANCE
#define OS_EXIT_CODE 0
#define LOG(MESSAGE) OutputDebugStringA("INFO: " MESSAGE "\n")

namespace os
{
    struct Instance {};
}

#elif OS_ANDROID  // --------------------------------------------------

#include <android/log.h>
#include <android/native_activity.h>
#define OS_MAIN() void android_main(struct android_app* app)
#define OS_INSTANCE  app
#define OS_EXIT_CODE // No exit code for android (very strange)
#define LOG(MESSAGE) __android_log_write(ANDROID_LOG_INFO, "CPP", MESSAGE);

struct android_app;
namespace os
{
    using Instance = android_app*;
}

#endif // ------- // --------------------------------------------------
