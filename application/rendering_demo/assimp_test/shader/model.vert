#version 450

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec3 _tangent;
layout(location = 3) in vec3 _bitangent;
layout(location = 4) in vec2 _texCoord;

layout(binding = 0) uniform MVTransform
{
    mat4 model;
    mat4 view;
} mv;

layout(binding = 1) uniform PTransform
{
    mat4 proj;
} p;

layout(location = 0) out vec2 texCoord;

void main()
{
	texCoord = _texCoord;
    gl_Position = p.proj * mv.model * vec4(_position, 1.0);
}