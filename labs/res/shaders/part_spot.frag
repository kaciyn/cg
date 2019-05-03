// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light
{
	vec4 light_colour;
	vec3 position;
	vec3 direction;
	float constant;
	float linear;
	float quadratic;
	float power;
};
#endif

// Material data
#ifndef MATERIAL
#define MATERIAL 
struct material
{
	vec4 emissive;
	vec4 diffuse_reflection;
	vec4 specular_reflection;
	float shininess;
};
#endif

// Spot light calculation
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
   // *********************************
  // Calculate light direction to position
 vec3 light_dir=normalize(spot.position-position);

  // Calculate distance to light
 float d=distance(spot.position,position);

  // Calculate attenuation value
 float att=spot.constant+(spot.linear*d)+(spot.quadratic*d*d);

  // Calculate spot light intensity
  float spot_intensity=pow(max(dot(-spot.direction,light_dir),0),spot.power);

  // Calculate light colour
	vec4 light_colour=spot.light_colour*spot_intensity/att;
	
  // Now use standard phong shading but using calculated light colour and direction
  // - note no ambient
     // Calculate diffuse component
   // Calculate k
  float k = max(dot(normal, light_dir), 0.0);

  // Calculate diffuse
  vec4 diffuse = k * (mat.diffuse_reflection * light_colour);

  // Calculate half vector
  vec3 half_vector=normalize(light_dir+view_dir);

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
  secondary.a = 1.0f;
  light_colour.a=1.0f;

  //calculate colour
  vec4 colour=primary*tex_colour+secondary;

	return colour;
}