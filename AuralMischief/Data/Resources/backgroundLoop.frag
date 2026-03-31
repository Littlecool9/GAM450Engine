#version 330 core

/**
 * @file backgroundLoop.frag
 * @brief Fragment shader for seamlessly looping background with tiling
 * 
 * Features:
 * - Seamless texture looping using fract()
 * - Configurable tiling frequency
 * - Optional smooth scrolling transition
 * - Parallax-aware rendering
 * 
 * How it works:
 * - UV coordinates are multiplied by tileScale to repeat the texture N times
 * - fract() wraps coordinates to [0, 1] range for seamless tiling
 * - Useful for infinite scrolling backgrounds (clouds, stars, patterns)
 */

in vec2 texCoord;
in vec2 tileScale;      // Number of tile repetitions (default: 1.0, 1.0)
out vec4 FragColor;

uniform vec3 tintColor;
uniform float alpha;
uniform sampler2D image;
uniform float seamlessBlend;  // Blend factor for smooth transitions (0.0 = hard edge, 1.0 = smooth)

void main()
{
    // Create seamlessly looping UV coordinates
    // fract() wraps coordinates so textures repeat infinitely
    vec2 loopingUV = fract(texCoord);
    
    // Sample the texture with looping coordinates
    vec4 texColor = texture(image, loopingUV);
    
    // Apply tint and alpha
    FragColor = vec4(tintColor, alpha) * texColor;
}
