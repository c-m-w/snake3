/// camera.hpp

#pragma once

class camera: public singleton<camera>, dependable<input>
{
protected:

    float fov = 45.f;
    float angle = 0.f;
    float distance = 5.f;
    float height = 1.f;

public:

    camera();

    glm::vec3 view_pos();
};
