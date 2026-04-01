#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec2  direction;   // (1,0) for horizontal, (0,1) for vertical
uniform int   radius;      // kernel half-size (1..15)
uniform float weights[16]; // precomputed gaussian weights
uniform vec2  texelSize;   // 1.0 / textureSize

void main()
{
    // Centre tap
    vec4 result = texture(screenTexture, texCoord) * weights[0];

    for (int i = 1; i <= radius; ++i)
    {
        vec2 offset = direction * texelSize * float(i);
        result += texture(screenTexture, texCoord + offset) * weights[i];
        result += texture(screenTexture, texCoord - offset) * weights[i];
    }

    FragColor = result;
}
