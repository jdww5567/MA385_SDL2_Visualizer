#version 410 core

uniform float uOffset;

in vec3 vColors;

out vec4 color;

void main() {
    color = vec4(vColors.r, vColors.g, vColors.b, 1.0f);
}