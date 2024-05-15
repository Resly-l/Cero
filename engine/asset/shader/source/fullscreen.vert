#version 450

layout(location = 0) in vec2 _position;

layout(location = 0) out vec2 texCoord;

void main()
{
    gl_Position = vec4(_position, 0.0, 1.0);
    texCoord = vec2((_position.x + 1.0f) * 0.5f, (_position.y + 1.0f) * 0.5f);
}