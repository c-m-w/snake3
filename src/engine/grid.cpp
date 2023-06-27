/// grid.cpp

#include "../snake3.hpp"

grid::grid()
{
    std::vector<std::uint16_t> indices;
    std::vector<vertex> vertices;

    indices.emplace_back(0);
    indices.emplace_back(1);
    vertices.emplace_back(
        vertex({0, 0, 0}, {1.f, 1.f, 1.f, 1.f})
    );
    vertices.emplace_back(
        vertex({0, 1, 1}, {1.f, 0.f, 1.f, 1.f})
    );

    index_buffer = device_buffer(indices.data(), indices.size());
    vertex_buffer = device_buffer(vertices.data(), vertices.size());
}

void grid::enqueue()
{
    render::get()->draw_buffer(index_buffer, vertex_buffer, DRAWMODE_LINE);
}
