#version 450

layout(location = 0) in vec3 _position;
layout(location = 1) in vec3 _normal;
layout(location = 2) in vec2 _texCoord;

layout(set = 0, binding = 0) uniform sampler2D DiffuseMap;
layout(set = 0, binding = 1) uniform sampler2D NormalMap;

layout(location = 0) out vec4 color;

void main()
{
	vec4 baseColor = texture(DiffuseMap, _texCoord);
	vec3 lightPosition  = vec3(0, 1, 0);
	vec3 lightDirection = normalize(_position - lightPosition);

	color  = baseColor * -dot(_normal, lightDirection);
}