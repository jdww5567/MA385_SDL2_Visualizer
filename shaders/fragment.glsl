#version 460 core

in vec3 vColors;

out vec4 color;

void main() {
    color = vec4(vColors.r, vColors.g, vColors.b, 0.8f);
}