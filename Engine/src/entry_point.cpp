#include <platform.hpp>
#include <engine.hpp>

OS_MAIN()
{
    os::Instance instance { OS_INSTANCE };

    if (engine.create(instance, on_create()))
        return OS_EXIT_CODE;

    engine.run();

    if (engine.destroy())
        return OS_EXIT_CODE;

    return OS_EXIT_CODE;
}
