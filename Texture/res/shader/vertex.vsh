#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 color;
layout (location = 2) in vec2 texCoord;

out vec4 M_Color;
out vec2 M_TexCoord;

void main(void)
{
    gl_Position = vec4(pos, 1.0);
    M_Color = vec4(color, 1.0);
    // gl_Position = pos;
    // M_Color = color;
    M_TexCoord = texCoord;
}
