// Directional light structure
#ifndef DIRECTIONAL_LIGHT
#define DIRECTIONAL_LIGHT
struct directional_light
{
	vec4 ambient_intensity;
	vec4 light_colour;
	vec3 light_dir;
};
#endif

// A material structure
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

// Calculates the directional light
vec4 calculate_direction(in directional_light light, in material mat, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
 // *********************************
	// Calculate ambient component
  vec4 ambient=mat.diffuse_reflection* light.ambient_intensity;

	// Calculate diffuse component :  (diffuse reflection * light.light_colour) *  max(dot(normal, light direction), 0)

  // Calculate k
  float k = max(dot(normal, light.light_dir), 0.0);

  // Calculate diffuse
  vec4 diffuse = k * (mat.diffuse_reflection * light.light_colour);

	// Calculate normalized half vector 
  // Calculate half vector between view_dir and light.light_dir
  vec3 H=normalize(light.light_dir+view_dir);

	// Calculate specular component : (specular reflection * light.light_colour) * (max(dot(normal, half vector), 0))^mat.mat.shininess
	float n_h=dot(normal,H);
  float max_n_h=max(n_h,0.0);
  float specular_intensity=pow(max_n_h,mat.shininess);
  vec4 specular=light.light_colour*specular_intensity;

 // *********************************
	// Calculate colour to return
	vec4 colour = ((mat.emissive + ambient + diffuse) * tex_colour) + specular;
	colour.a = 1.0;
	// Return colour
	return colour;

}
