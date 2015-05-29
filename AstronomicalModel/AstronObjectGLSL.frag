#version 330
in vec4 colour;
in vec2 textureSTMapFrag;
uniform sampler2D sample01;
out vec4 fColor;

void main() {
    if (instanceID==0)
        fColor = texture(sample01, textureSTMapFrag);
    else
        fColor = colour;
}