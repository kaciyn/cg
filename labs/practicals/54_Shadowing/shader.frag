#version 440

// This shader requires spot.frag, part_shadow.frag

// Spot light data
#ifndef SPOT_LIGHT
#define SPOT_LIGHT
struct spot_light {
  vec4 light_colour;
  vec3 position;
  vec3 direction;
  float constant;
  float linear;
  float quadratic;
  float power;
};
#endif

// A material structure
#ifndef MATERIAL
#define MATERIAL
struct material {
  vec4 emissive;
  vec4 diffuse_reflection;
  vec4 specular_reflection;
  float shininess;
};
#endif

// Forward declarations of used functions
vec4 calculate_spot(in spot_light spot, in material mat, in vec3 position, in vec3 normal, in vec3 view_dir,
                    in vec4 tex_colour);
float calculate_shadow(in sampler2D shadow_map, in vec4 light_space_pos);

// Spot light being used in the scene
uniform spot_light spot;
// Material of the object being rendered
uniform material mat;
// Position of the eye
uniform vec3 eye_pos;
// Texture to sample from
uniform sampler2D tex;
// Shadow map to sample from
uniform sampler2D shadow_map;

// Incoming position
layout(location = 0) in vec3 position;
// Incoming normal
layout(location = 1) in vec3 normal;
// Incoming texture coordinate
layout(location = 2) in vec2 tex_coord;
// Incoming light space position
layout(location = 3) in vec4 light_space_pos;

// Outgoing colour
layout(location = 0) out vec4 colour;

void main() {
  // *********************************
  // Calculate shade factor
  float shade_factor=calculate_shadow(shadow_map,light_space_pos);
  // Calculate view direction, normalize it
  vec3 view_dir=normalize(eye_pos-position);

  // Sample texture
  vec4 tex_colour=texture(tex,tex_coord);

  // Calculate spot light

   // *********************************
  // Calculate direction to the light
  vec3 dir=normalize(spot.position-position);

  // Calculate distance to light
 float d=distance(spot.position,position);

  // Calculate attenuation value
  float kds=(spot.constant+(spot.linear*d)+(spot.quadratic*pow(d,2)));
  vec4 att=(1/kds)*spot.light_colour;

  // Calculate spot light intensity
  vec4 spot_intensity=att*pow(max(dot(spot.direction,-dir),0),spot.power);

  // Calculate light colour
	vec4 light_colour=spot.light_colour;
  // *********************************


  // Scale colour by shade
 light_colour= light_colour*shade_factor;
  //Ensure alpha is 1.0
  light_colour.a=1.0f;
  // *********************************
}