#version 330 core

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uvCoord;

out vec2 texCoord;
out vec3 color;

uniform mat4 model;
uniform mat4 viewProj;
uniform vec2 uvOffset;

void main()
{
    texCoord = uvCoord + uvOffset + vec2(0.0, 0.09);
    vec4 world = viewProj * model * vec4(pos, 0.0, 1.0);
    gl_Position = world;
}
