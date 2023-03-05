#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colors;

uniform float uVertOffset;
uniform float uHoriOffset;
uniform float uDepthOffset;

out vec3 vColors;

void main() {
   gl_Position = vec4(position.x + uHoriOffset, position.y + uVertOffset, position.z + uDepthOffset, 1.0f);

   vColors = colors;
}