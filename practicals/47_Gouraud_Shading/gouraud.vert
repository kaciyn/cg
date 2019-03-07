#version 440

// A directional light structure
struct directional_light {
  vec4 ambient_intensity;
  vec4 light_colour;
  vec3 light_dir;
};

// A material structure
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// The model matrix
uniform mat4 M;
// The transformation matrix
uniform mat4 MVP;
// The normal matrix
uniform mat3 N;
// Directional light for the scene
uniform directional_light light;
// mat of the object
uniform material mat;
// Position of the camera
uniform vec3 eye_pos;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 2) in vec3 normal;
// Incoming texture coordinate
layout(location = 10) in vec2 tex_coord_in;

// Outgoing primary colour
layout(location = 0) out vec4 primary;
// Outgoing secondary colour
layout(location = 1) out vec4 secondary;
// Outgoing texture coordinate
layout(location = 2) out vec2 tex_coord_out;

void main() {
  // *********************************
  // Calculate position
  gl_Position=MVP*vec4(position,1.0);

  // Calculate ambient component
  vec4 ambient=dot(material.diffuse_reflection,light.ambient_intensity,0);

  // Transform the normal
  vec3 transformed_normal=N*normal;

  // Calculate k
 // float k=max(dot(normal,light_dir),0.0);
  //k is here twice? do i need to do it? it's not in the algorithm

  // Calculate diffuse
 vec4 diffuse=max(dot(transformed_normal,light.light_dir,0))*(material.diffuse_reflection*light.light_colour);
 //is this a dot product of the two or mult?

  // Calculate world position of vertex
  vec3 world_position=mul(position,M);

  // Calculate view direction
  vec3 view_dir=eye_pos-world_position;

  // Calculate half vector between view_dir and light_dir
  vec3 half_vector=light.light_dir+view_dir;

   // Calculate specular
   vec4 specular=pow(max(dot(transformed_normal,half_vector,0)),mat.shininess)*(mat.specular_reflection*light.light_colour);

  // Set primary
  vec4 primary=mat.emissive+ambient+material.diffuse_reflection;

  // Set secondary
  vec4 secondary=specular;

  // Ensure primary and secondary alphas are 1
  primary.a=1;
  secondary.a=1;

  // Pass through texture coordinate
  tex_coord_out=tex_coord_in;
  // *********************************
}