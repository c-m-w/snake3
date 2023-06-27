/// grid.hpp

#pragma once

class grid: public i_renderable, public singleton<grid>
{
private:

    static constexpr auto NCELLS = 10;
    static constexpr auto CELL_WIDTH = 1.f;

    device_buffer index_buffer;
    device_buffer vertex_buffer;

public:

    grid();

    void enqueue() override;
};
