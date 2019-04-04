// Point light information
#ifndef POINT_LIGHT
#define POINT_LIGHT
struct point_light
{
	vec4 light_colour;
	vec3 position;
	float constant;
	float linear;
	float quadratic;
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

// Point light calculation
vec4 calculate_point(in point_light point, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
	// *********************************
	// Get distance between point light and vertex
 float d=distance(point.position,position);

	// Calculate attenuation factor : constant + (linear * d) + (quadratic * d * d)
	 float kds=point.constant+(point.linear*d)+(point.quadratic*pow(d,2));
  vec4 att=(1/kds)*point.light_colour;


	// Calculate light colour : light_colour / attenuation
  vec4 light_colour=point.light_colour*att;

	//Set colour alpha to 1.0
	light_colour.a=1.0f;
	// Calculate light dir
  vec3 dir=normalize(point.position-position);

	// *********************************
	// Now use standard phong shading but using calculated light colour and direction
	// - note no ambient
  float k = max(dot(normal, dir), 0.0);

  vec4 diffuse = k * (mat.diffuse_reflection * light_colour);
  vec3 half_vector=normalize(dir+view_dir);
	vec4 specular = (mat.specular_reflection * light_colour) * pow(max(dot(normal, half_vector), 0), mat.shininess);
	vec4 primary = mat.emissive + diffuse;
	vec4 colour = primary * tex_colour + specular;
	colour.a = 1.0;
	return colour;
}