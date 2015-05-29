#version 330
in vec4 colour;
in vec2 textureSTMapFrag;
uniform sampler2D sample;
out vec4 fColor;

void main() {
//    fColor =  colour * texture(sample, textureSTMapFrag);
    fColor =  colour;
}