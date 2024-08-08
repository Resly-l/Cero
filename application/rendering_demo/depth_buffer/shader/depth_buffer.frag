#version 450

layout (location = 0) in vec3 _color;

layout (location = 0) out vec4 color;

void main()
{
	color = vec4(_color, 1.0f);
}