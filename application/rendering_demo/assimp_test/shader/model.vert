#version 450

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec3 _tangent;
layout(location = 3) in vec3 _bitangent;
layout(location = 4) in vec2 _texCoord;

layout(set = 1, binding = 2) uniform MVTransform
{
    mat4 model;
    mat4 view;
} mv;

layout(set = 1, binding = 3) uniform PTransform
{
    mat4 proj;
} p;

layout(location = 0) out vec3 position;
layout(location = 1) out vec3 normal;
layout(location = 2) out vec2 texCoord;

void main()
{
	position = (mv.model * vec4(_position, 1.0)).xyz;
	normal = (mv.model * vec4(_normal, 1.0)).xyz;
	texCoord = _texCoord;

    gl_Position = p.proj * vec4(position, 1.0);
}