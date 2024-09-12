#include <engine.hpp>

Engine engine{
    69};

auto Engine::create(os::Instance const &instance, int _) -> int
{
    LOG(__FUNCTION__);
    return 0;
}

auto Engine::run() -> void
{
    LOG(__FUNCTION__);

#if OS_ANDROID
    LOG("ANDROID IS WORKING!!!");
#endif
}

auto Engine::destroy() -> int
{
    LOG(__FUNCTION__);
    return 0;
}
