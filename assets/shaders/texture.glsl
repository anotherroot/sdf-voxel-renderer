#type vertex
#version 330 core
layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec2 a_tex_coords;

out vec2 v_tex_coords;

void main(){
  gl_Position = vec4(a_pos,1.0);
  v_tex_coords = a_tex_coords;
}

#type fragment
#version 330 core
out vec4 out_color;

in vec2 v_tex_coords;

uniform sampler2D u_albedo;
uniform sampler2D u_debug;

void main(){
  /* out_color = vec4(v_tex_coords,1,1); */
  out_color = texture(u_albedo, v_tex_coords);
  vec4 debug = texture(u_debug,v_tex_coords);
  if(debug.a>0.5){
    out_color = debug;
  }
}
