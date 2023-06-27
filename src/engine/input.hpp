/// input.hpp

#pragma once

using callback_fn = std::function<void()>;

struct i_callback
{
    virtual void try_call() = 0;
};

struct single_callback: i_callback
{
    callback_fn cb;

    single_callback(callback_fn cb):
        cb(cb)
    { }

    void try_call() override
    {
        cb();
    }
};

struct interval_callback: i_callback
{
    callback_fn cb;
    std::uint64_t interval;
    std::uint64_t last_call;

    interval_callback(callback_fn cb, std::uint64_t interval):
        cb(cb), interval(interval), last_call(0ull)
    { }

    void try_call() override
    {
        auto const & now = utils::now();

        if (now - last_call < interval)
            return;

        cb();
        last_call = now;
    }
};

class input: public singleton<input>, dependable<window>
{
public:

private:

    enum key_state
    {
        STATE_UP,
        STATE_DOWN
    };  

    std::array<key_state, GLFW_KEY_LAST + 1> key_states {STATE_UP};
    std::unordered_map<int, std::vector<single_callback>> single_callbacks;
    std::unordered_map<int, std::vector<interval_callback>> interval_callbacks;

public:

    static void key_callback(void * window, int key, int scan_code, int action, int mods);

    input();

    void add_callback(int key, single_callback const & s);
    void add_callback(int key, interval_callback const & i);
    void process_repeats();
};