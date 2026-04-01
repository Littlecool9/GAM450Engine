#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform vec3 tintColor;

void main()
{
    vec4 color = texture(screenTexture, texCoord);
    FragColor = vec4(color.rgb * tintColor, color.a);
}
