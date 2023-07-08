/// grid.hpp

#pragma once

class grid: public i_renderable, public singleton<grid>
{
private:

    static constexpr auto NCELLS = 31;
    static constexpr auto CELL_WIDTH = 1.f;
    static constexpr auto MOVE_TIME = 2000u;

    device_buffer grid_indices;
    device_buffer grid_vertices;
    std::vector<device_buffer> tail_buffers;
    std::pair<device_buffer, device_buffer> head;

    glm::uvec2 pos {
        static_cast<std::uint32_t>(NCELLS / 2.f) + 1,
        static_cast<std::uint32_t>(NCELLS / 2.f) + 1
    };
    std::vector<glm::uvec2> tail {
        glm::uvec2 {
            static_cast<std::uint32_t>(NCELLS / 2.f) + 1,
            static_cast<std::uint32_t>(NCELLS / 2.f) + 1
        }
    };
    glm::uvec2 move {1, 0};
    std::uint64_t last_move_time = 0ull;

    float get_z(float x, float y);

public:

    grid();

    void enqueue() override;
};
