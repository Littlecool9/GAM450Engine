#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform sampler2D prevFrameTexture;
uniform float strength;  // 0 = no blur, closer to 1 = heavier trail

void main()
{
    vec4 current = texture(screenTexture, texCoord);
    vec4 prev    = texture(prevFrameTexture, texCoord);
    FragColor    = mix(current, prev, strength);
}
