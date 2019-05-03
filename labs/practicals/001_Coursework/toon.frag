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
vec4 calculate_toon(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir, in vec4 tex_colour)
{
    // *********************************
    // Calculate light direction to position
    vec3 light_dir = normalize(spot.position - position);

    // Calculate distance to light
    float d = distance(spot.position, position);

    // Calculate attenuation value
    float att = spot.constant + (spot.linear * d) + (spot.quadratic * d * d);

    // Calculate spot light intensity
    float spot_intensity = pow(max(dot(-spot.direction, light_dir), 0), spot.power)/att;

    // Calculate light colour
    vec4 light_colour = spot.light_colour * spot_intensity;

    // Now use standard phong shading but using calculated light colour and direction
    // - note no ambient
    // Calculate diffuse component
    // Calculate k
    float k = max(dot(normal, light_dir), 0.0);

    // Calculate diffuse
    vec4 diffuse = k * (mat.diffuse_reflection * light_colour);

	vec4 colour;
    //Diffuse illumination
  float intensity;
	if (spot_intensity > 0.95f)
    {
        intensity = 1.0f;
    }
	  else if (spot_intensity > 0.5f)
    {
        intensity = 0.6f;
    }
           else if (spot_intensity > 0.25f)
    {
        intensity = 0.4f;
    }
    else if (spot_intensity > 0.15f)
    {
       intensity = 0.2f;
    }
    else
    {
        intensity = 0.05;
    }

	colour =  tex_colour*intensity;

    // Outline
    if (dot(view_dir, normal) < mix(0.5f, 0.2f,k))
    {
        colour =  vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

       return colour;
}