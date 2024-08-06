#version 460 core

out vec4 FragColor;

uniform float colorR;
uniform float colorG;
uniform float colorB;

void main() {
    FragColor = vec4(colorR, colorG, colorB, 1.0);
}