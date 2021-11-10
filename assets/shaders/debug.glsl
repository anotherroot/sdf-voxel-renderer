#type vertex
#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;

out vec3 v_color;
uniform mat4 u_view_projection;

void main(){
  gl_Position = u_view_projection*vec4(a_pos,1.0);
  v_color = a_color;
}

#type fragment
#version 330 core
out vec4 out_color;

in vec3 v_color;


void main(){
  out_color = vec4(v_color,1);
}
