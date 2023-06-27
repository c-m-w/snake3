/// input.cpp

#include "../snake3.hpp"

void input::key_callback(void * window, int key, int scan_code, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        for (auto const & s: input::get()->single_callbacks[key])
            s.cb();

        input::get()->key_states[key] = STATE_DOWN;
    }
    else if (action == GLFW_RELEASE)
        input::get()->key_states[key] = STATE_UP;

    
    input::get()->process_repeats();
}

input::input()
{
    glfwSetKeyCallback(*window::get(), reinterpret_cast<GLFWkeyfun>(key_callback));
}

void input::add_callback(int key, single_callback const & s)
{
    single_callbacks[key].emplace_back(s);
}

void input::add_callback(int key, interval_callback const & i)
{
    interval_callbacks[key].emplace_back(i);
}

void input::process_repeats()
{
    for (auto & [key, repeats]: interval_callbacks)
    {
        if (key_states[key] != STATE_DOWN)
            continue;

        for (auto & r: repeats)
            r.try_call();
    }
}
