#version 330 core

layout(location = 0) in vec2 a_Pos;
layout(location = 1) in vec2 texCoord;
layout(location = 2) in vec3 color;

out vec2 TexCoords;
out vec3 v_Color;

uniform mat3 u_Model;
uniform mat3 u_ViewProj;

void main()
{
    TexCoords = texCoord;
    v_Color = color;
    vec3 world = u_ViewProj * u_Model * vec3(a_Pos, 1.0);
    gl_Position = vec4(world.xy, 0.0, 1.0);
}
