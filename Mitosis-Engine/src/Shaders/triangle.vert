#version 330 core

layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 color;

out vec2 TexCoords;
out vec3 v_Color;

uniform mat4 u_Model;
uniform mat4 u_ViewProj;

void main()
{
    TexCoords = texCoord;
    v_Color = color;
    vec4 world = u_ViewProj * u_Model * vec4(a_Pos, 0.0, 1.0);
    gl_Position = world;
}
