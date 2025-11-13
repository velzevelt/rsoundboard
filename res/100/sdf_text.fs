#version 100

precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;

const float smoothing = 1.0 / 16.0;
// const float outlineWidth = 3.0 / 16.0;
uniform float outlineWidth;

uniform vec4 outlineColor;

// ref: https://stackoverflow.com/questions/26155614/outlining-a-font-with-a-shader-and-using-distance-field

void main()
{
    float outerEdgeCenter = 0.5 - outlineWidth;

    float distance = texture2D(texture0, fragTexCoord).a;
    float alpha = smoothstep(outerEdgeCenter - smoothing, outerEdgeCenter + smoothing, distance);
    float border = smoothstep(0.5 - smoothing, 0.5 + smoothing, distance);

    vec4 color = vec4(
        mix(outlineColor.rgb, fragColor.rgb, border), 
        mix(outlineColor.a, fragColor.a, border) * alpha
    );

    gl_FragColor = color;
}