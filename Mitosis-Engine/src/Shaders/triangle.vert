#version 330 core

layout(location = 0) in vec2 a_Pos;

uniform mat3 u_Model;
uniform mat3 u_ViewProj;

void main()
{
    vec3 world = u_ViewProj * u_Model * vec3(a_Pos, 1.0);
    gl_Position = vec4(world.xy, 0.0, 1.0);
}
