#version 330 core

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;

const float smoothing = 1.0 / 16.0;
uniform float outlineWidth;

uniform vec4 outlineColor;

out vec4 finalColor;

void main()
{
    float outerEdgeCenter = 0.5 - outlineWidth;

    float distance = texture(texture0, fragTexCoord).a;
    float alpha = smoothstep(outerEdgeCenter - smoothing, outerEdgeCenter + smoothing, distance);
    float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);

    vec4 color = vec4(
        mix(outlineColor.rgb, fragColor.rgb, border), 
        mix(outlineColor.a, fragColor.a, border) * alpha
    );

    finalColor = color;
}