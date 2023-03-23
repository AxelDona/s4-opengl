#version 330 core

in vec3 vFragColor;

out vec3 fFragColor;

void main() {
    fFragColor[0] = (vFragColor[0] + vFragColor[1] + vFragColor[2]) / 3;
    fFragColor[1] = (vFragColor[0] + vFragColor[1] + vFragColor[2]) / 3;
    fFragColor[2] = (vFragColor[0] + vFragColor[1] + vFragColor[2]) / 3;
};