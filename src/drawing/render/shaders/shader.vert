#version 450

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
    gl_Position = vec4(positions[gl_VertexIndex], 0.0, 1.0);
    out_color = vec4(colors[gl_VertexIndex], 1.0);
    gl_PointSize = 40;
    gl_Position = vec4(0,0,0,0);
}