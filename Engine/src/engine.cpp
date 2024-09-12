#include <engine.hpp>

Engine engine {};
Graphics gfx {};

extern void main_loop();

auto Engine::create(os::Instance const &instance, int _) -> int
{
    os::initialize();
    return 0;
}

auto Engine::run() -> void
{
    LOGI("running!");
#if OS_ANDROID
    LOGI("ANDROID IS WORKING!!!");
#endif
    main_loop();
}

auto Engine::destroy() -> int
{
    os::shutdown();
    return 0;
}
