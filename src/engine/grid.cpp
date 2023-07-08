/// grid.cpp

#include "../snake3.hpp"

float grid::get_z(float x, float y)
{
    return -x + y / 4.f + std::cos(x / 2.f) + std::sin(y / 2.f);
}

grid::grid():
    last_move_time(utils::now())
{
    std::vector<std::uint16_t> indices { };
    std::vector<vertex> vertices { };

    for (auto x = 0; x <= NCELLS; x++)
        for (auto y = 0; y <= NCELLS; y++)
        {
            auto const world_x = (x - NCELLS / 2.f) * CELL_WIDTH;
            auto const world_y = (y - NCELLS / 2.f) * CELL_WIDTH;

            vertices.emplace_back(
                vertex({
                        world_x,
                        world_y,
                        get_z(world_x, world_y)}, 
                        {0.f, y / static_cast<float>(NCELLS), x / static_cast<float>(NCELLS), 1.f
                })
            );
        }

    for (auto i = 0; i < NCELLS; i++)
        for (int j = 0; j <= NCELLS; j++)
        {
            indices.emplace_back(i + j * (NCELLS + 1));
            indices.emplace_back(i + 1 + j * (NCELLS + 1));
            indices.emplace_back(i * (NCELLS + 1) + j);
            indices.emplace_back((i + 1) * (NCELLS + 1) + j);
        }

    grid_indices = device_buffer(indices.data(), indices.size());
    grid_vertices = device_buffer(vertices.data(), vertices.size());

    camera::get()->set_distance(NCELLS * CELL_WIDTH * 2.f);
}

void grid::enqueue()
{
    std::array<std::pair<std::int32_t, std::int32_t>, 4> cycle
    {
        std::make_pair(-1, -1),
        std::make_pair(0, -1),
        std::make_pair(-1, 0),
        std::make_pair(0, 0)
    };

    auto const fraction = static_cast<float>(utils::now() - last_move_time) / MOVE_TIME;

    std::array<vertex, 4> head_vertices;
    std::vector<std::array<vertex, 4>> tail_vertices;

    for (auto s = 0u; s < head_vertices.size(); s++)
    {
        auto const world_x = (pos.x + move.x + cycle[s].first) * CELL_WIDTH - NCELLS / 2.f;
        auto const world_y = (pos.y + move.y + cycle[s].second) * CELL_WIDTH - NCELLS / 2.f;

        head_vertices[s] = vertex(glm::vec3(world_x, world_y, get_z(world_x, world_y)), glm::vec4(1.f, 1.f, 1.f, fraction));
    }

    tail_buffers.clear();

    for (auto const & t : tail)
    {
        auto & v = tail_vertices.emplace_back();

        for (auto s = 0u; s < cycle.size(); s++)
        {
            auto const world_x = (t.x + cycle[s].first) * CELL_WIDTH - NCELLS / 2.f;
            auto const world_y = (t.y + cycle[s].second) * CELL_WIDTH - NCELLS / 2.f;

            v[s] = vertex(glm::vec3(world_x, world_y, get_z(world_x, world_y)), glm::vec4(1.f, 1.f, 1.f, 1.f));
        }

        tail_buffers.emplace_back(device_buffer(v.data(), v.size()));
    }

    std::array<std::uint16_t, 6> indices
    {
        0, 1, 2, 1, 2, 3
    };

    head.first = device_buffer(head_vertices.data(), head_vertices.size());
    head.second = device_buffer(indices.data(), indices.size());

    for (auto & square: tail_buffers)
        render::get()->draw_buffer(square, head.second, DRAWMODE_FILL);

    render::get()->draw_buffer(head.first, head.second, DRAWMODE_FILL);
    render::get()->draw_buffer(grid_vertices, grid_indices, DRAWMODE_LINE);

    if (fraction >= 1.f) {

        pos += move;
        tail.emplace_back(pos);
        last_move_time = utils::now();
    }
}
