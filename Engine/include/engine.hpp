#pragma once
#include <platform.hpp>

struct Engine {
    int a;

    auto create  (os::Instance const&, int) -> int;
    auto run     () -> void;
    auto destroy () -> int;
};

extern Engine engine;

extern int on_create();
