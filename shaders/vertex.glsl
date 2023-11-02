#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 colors;

uniform mat4 uViewMatrix;

out vec4 vColors;

void main() {
   vec4 viewPosition = uViewMatrix * vec4(position, 1.0f);
   
   gl_Position = viewPosition;

   vColors = colors;
}