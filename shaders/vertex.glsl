#version 460 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 colors;

uniform mat4 u_view_matrix;

out vec4 v_colors;

void main() {
   vec4 view_position = u_view_matrix * vec4(position, 1.0f);
   
   gl_Position = view_position;

   v_colors = vec4(colors, 1.0f);
}