#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform vec3 tintColor;
uniform float alpha;
uniform sampler2D image;

void main()
{
    vec4 color = vec4(tintColor, alpha) * texture(image, texCoord);
    color.rgb = vec3(1.0) - color.rgb;
    FragColor = color;
}
