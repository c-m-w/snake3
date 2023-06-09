#version 450

layout( binding = 0 ) uniform uniform_buffer_object_t {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout( location = 0 ) in vec3 position;
layout( location = 1 ) in vec4 color;

layout( location = 0 ) out vec4 out_color;

vec2 positions[3] = vec2[](
    vec2(0.0, -0.5),
    vec2(0.5, 0.5),
    vec2(-0.5, 0.5)
);

vec3 colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main( )
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(position, 1.0);
    out_color = color;
    gl_PointSize=50;
}