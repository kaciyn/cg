#version 440

// The transformation matrix
uniform mat4 MVP;
// Material colour
uniform vec4 material_colour;
// Light colour
uniform vec4 light_colour;
// Direction of the light
uniform vec3 light_dir;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;

// Outgoing vertex colour
layout(location = 0) out vec4 vertex_colour;

void main() {
  // *********************************
  // Calculate position
  gl_Position=MVP*vec4(position,1.0);

  // Calculate k
  float k=max(dot(normal,light_dir),0.0);
  // Calculate diffuse
 vec4 diffuse=k*(material_colour*light_colour);
  // Ensure alpha is 1
  diffuse.a=1;
  // Output vertex colour - just diffuse
  vertex_colour=diffuse;
  // *********************************
}