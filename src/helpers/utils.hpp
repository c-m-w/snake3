/// utils.hpp

#pragma once

namespace utils
{
    inline void sleep(unsigned long long duration_ms)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(duration_ms));
    }

    inline std::vector<char> load_file(std::string const & file_name)
    {
        std::ifstream f(file_name, std::ios::ate | std::ios::binary);

        if (!f.good())
            return {};

        auto const size = static_cast<std::size_t>(f.tellg());
        std::vector<char> buffer(size);

        f.seekg(0);
        f.read(&buffer[0], size);
        f.close();

        return buffer;
    }
}
