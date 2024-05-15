#version 450

layout (binding = 1) uniform sampler2D positionGBuffer;
layout (binding = 2) uniform sampler2D normalGBuffer;
layout (binding = 3) uniform sampler2D colorGBuffer;

layout(location = 0) in vec2 _texCoord;

layout(location = 0) out vec4 color;

void main()
{
    color = vec4(1.0f);

	color = texture(colorGBuffer, _texCoord);
}