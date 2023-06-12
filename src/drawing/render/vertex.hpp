/// vertex.hpp

#pragma once

class vertex
{
private:

    glm::vec3 pos;
    glm::vec4 col;

public:

    vertex(glm::vec3 const & pos, glm::vec4 const & col);

    static vk::VertexInputBindingDescription
    get_binding_description( );
    static std::array<vk::VertexInputAttributeDescription, 2>
    get_attribute_descriptions( );
};
