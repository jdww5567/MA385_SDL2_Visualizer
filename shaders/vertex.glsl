#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colors;

uniform mat4 uModelMatrix;

out vec3 vColors;

void main() {
   vec4 translatedPosition = uModelMatrix * vec4(position, 1.0f);

   gl_Position = translatedPosition;

   vColors = colors;
}