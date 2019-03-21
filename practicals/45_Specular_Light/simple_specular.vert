#version 440

// The model matrix
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;

// The normal matrix
uniform mat3 N;

// Material colour
uniform vec4 material_colour;

// Shininess of the object
uniform float shininess;

// Light colour
uniform vec4 light_colour;

// Direction of the light
uniform vec3 light_dir;

// Position of the camera
uniform vec3 eye_pos;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;

// Outgoing vertex colour
layout(location = 0) out vec4 vertex_colour;

void main() {
  // *********************************
  // Calculate position
  gl_Position=MVP*vec4(position,1.0f);

  // Transform the normal
  vec3 transformed_normal=N*normal;

  // Calculate world position
  //cast position up to vec4
  vec4 world_pos=M*vec4(position,1);;

  // Calculate view direction
  vec3 view_dir=normalize(eye_pos-position);
 
 //YOUR PROBLEM WAS HERE THAT YOU DIDNT HAVE TO FUCKING TO THE MINUS OVER THE NORMAL JUST NORMALISE IT GODDDDDD
 
 // Calculate half vector between view_dir and light_dir
  vec3 H=normalize(light_dir+view_dir);

  // Calculate k
  float k = max(dot(transformed_normal, light_dir), 0.0);

  // Calculate specular intensity
  float n_h=dot(transformed_normal,H);
  float max_n_h=max(n_h,0.0f);
  float specular_intensity=pow(max_n_h,shininess);

  // Calculate diffuse
  vec4 diffuse = k * (material_colour * light_colour);

  // Calculate specular
  vec4 specular=diffuse*specular_intensity;
 
  // Ensure alpha is 1
  diffuse.a=1.0f;
  specular.a = 1.0f;

  // Output vertex colour - just specular
  vertex_colour = specular;
  
  // *********************************
}