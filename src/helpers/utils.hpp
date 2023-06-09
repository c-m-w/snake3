/// utils.hpp

#pragma once

namespace utils
{
    inline void sleep(unsigned long long duration_ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    }
}
