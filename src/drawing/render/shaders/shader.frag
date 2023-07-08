#version 450

layout( location = 0 ) in vec4 in_color;

layout( location = 0 ) out vec4 out_color;

layout( push_constant ) uniform push_constants
{
    vec4 background;
} push;

void main( )
{
    out_color = in_color * in_color.a + push.background * (1.f - in_color.a);
}
