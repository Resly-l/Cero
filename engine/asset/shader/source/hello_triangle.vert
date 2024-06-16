#version 450

layout(location = 0) in vec2 _position;
layout(location = 1) in vec3 _color;

layout(binding = 0) uniform MVTransform
{
    mat4 model;
    mat4 view;
} mv;

layout(binding = 1) uniform PTransform
{
    mat4 proj;
} p;

layout(location = 0) out vec3 fragColor;

void main()
{
    gl_Position = p.proj * mv.model * vec4(_position, 0.0, 1.0);

    fragColor = _color;
}