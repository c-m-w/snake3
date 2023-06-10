/// utils.hpp

#pragma once

namespace utils
{
    inline void sleep(unsigned long long duration_ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    }

    template<bool ... Bs>
    constexpr inline bool eval_pack()
    {
        auto b = true;

        b &= (Bs, ...);

        return b;
    }
}
