#version 330 core
in vec4 M_Color;
in vec2 M_TexCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float mixNumber;        // [0, 1]

void main(void)
{
    //gl_FragColor = mix(texture2D(texture1, M_TexCoord * vec2(1.0, -1.0)), texture2D(texture2, M_TexCoord * vec2(1.0, -1.0)), mixNumber);
    gl_FragColor = texture2D(texture0, M_TexCoord);
    // gl_FragColor = M_Color;
}
