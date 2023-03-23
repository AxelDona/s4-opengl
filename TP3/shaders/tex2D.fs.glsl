#version 330 core

in vec2 vFragTexture;

out vec3 fFragColor;

uniform vec3 uColor;

void main() {
    fFragColor = uColor;
};