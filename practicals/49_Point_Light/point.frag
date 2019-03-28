#version 440

// Point light information
struct point_light {
  vec4 light_colour;
  vec3 position;
  float constant;
  float linear;
  float quadratic;
};

// Material information
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};

// Point light for the scene
uniform point_light point;
// Material for the object
uniform material mat;
// Eye position
uniform vec3 eye_pos;
// Texture
uniform sampler2D tex;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Get distance between point light and vertex
 float d=distance(point.position,position);

  // Calculate attenuation factor
 float kds=point.constant+(point.linear*d)+(point.quadratic*pow(d,2));
  vec4 att=(1/kds)*point.light_colour;

  // Calculate light colour
  vec4 light_colour=point.light_colour*att;

  // Calculate light dir
  vec3 dir=normalize(point.position-position);

  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
     // Calculate diffuse component

   // Calculate k
  float k = max(dot(normal, dir), 0.0);

  // Calculate diffuse
  vec4 diffuse = k * (mat.diffuse_reflection * light_colour);

  // Calculate half vector
  vec3 view_dir=normalize(eye_pos-position);
  vec3 half_vector=normalize(dir+view_dir);

  // Calculate specular component
  float n_h=dot(normal,half_vector);
  float max_n_h=max(n_h,0.0f);
  float specular_intensity=pow(max_n_h,mat.shininess);
  vec4 specular=specular_intensity*mat.specular_reflection*light_colour;

  // Calculate primary colour component
  vec4 primary=mat.emissive+diffuse;

  // Calculate final colour - remember alpha
  vec4 secondary=specular;

   //set alphas
  primary.a = 1.0f;
  secondary.a=1.0f;
  light_colour.a=1.0f;

  //calculate colour
  vec4 tex_colour=texture(tex,tex_coord);
  colour=primary*tex_colour+secondary;   
}