#version 450

layout (location = 0) in vec3 _position;
layout (location = 1) in vec3 _color;

layout (location = 0) out vec3 color;

layout(binding = 0) uniform MVTransform
{
    mat4 model;
    mat4 view;
} mv;

layout(binding = 1) uniform PTransform
{
    mat4 proj;
} p;

void main()
{
        gl_Position = p.proj * mv.model * vec4(_position, 1.0);
	color = _color;
}