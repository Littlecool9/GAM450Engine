#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform vec3 tintColor;
uniform float alpha;
uniform sampler2D image;

void main()
{
    FragColor = vec4(tintColor, alpha) * texture(image, texCoord);
}
