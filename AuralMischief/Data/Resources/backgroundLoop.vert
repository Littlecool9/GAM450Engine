#version 330 core

/**
 * @file backgroundLoop.vert
 * @brief Vertex shader for looping background with seamless tiling
 * 
 * Features:
 * - Support for parallax scrolling
 * - Tiling-ready UV coordinates
 * - Smooth continuous scrolling
 */

layout(location = 0) in vec2 pos;
layout(location = 1) in vec2 uvCoord;

out vec2 texCoord;
out vec3 color;
out vec2 tileScale;

uniform mat4 model;
uniform mat4 viewProj;
uniform vec2 uvOffset;
// uniform vec2 tileScale;      // How many times to tile the texture (default: 1.0, 1.0)

void main()
{
    // Apply tiling scale to UV coordinates
    tileScale = vec2(1.0, 1.0); // This can be set from the application to control tiling
    texCoord = (uvCoord * tileScale) + uvOffset;
    
    vec4 world = viewProj * model * vec4(pos, 0.0, 1.0);
    gl_Position = world;
}
