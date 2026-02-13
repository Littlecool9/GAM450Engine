#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform vec3 tintColor;
uniform float alpha;
uniform sampler2D image;

void main()
{
    vec4 texColor = vec4(tintColor, alpha) * texture(image, texCoord);
    FragColor = vec4(texColor.rgb * texColor.a, texColor.a);
}
