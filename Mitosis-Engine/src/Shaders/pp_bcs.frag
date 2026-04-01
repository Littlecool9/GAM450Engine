#version 330 core

in vec2 texCoord;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform float brightness;
uniform float contrast;
uniform float saturation;

void main()
{
    vec4 color = texture(screenTexture, texCoord);

    // Brightness (additive)
    color.rgb += brightness;

    // Contrast (scale around mid-grey)
    color.rgb = (color.rgb - 0.5) * contrast + 0.5;

    // Saturation (lerp to luminance)
    float luma = dot(color.rgb, vec3(0.2126, 0.7152, 0.0722));
    color.rgb = mix(vec3(luma), color.rgb, saturation);

    FragColor = vec4(clamp(color.rgb, 0.0, 1.0), color.a);
}
