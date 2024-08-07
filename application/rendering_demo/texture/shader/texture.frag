#version 450

layout (location = 0) in vec2 _texCoord;
layout (binding = 0) uniform sampler2D texSampler;

layout (location = 0) out vec4 color;

void main()
{
	color = texture(texSampler, _texCoord);
}