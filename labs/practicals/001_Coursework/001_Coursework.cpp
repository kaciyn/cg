#include <glm\glm.hpp>
#include <graphics_framework.h>
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> reflective_meshes;
map<string, mesh> meshes;
map<string, texture> textures;

mesh skybox;

effect eff;
effect reflective_eff;
effect sky_eff;
effect shadow_eff;
effect blur_eff;

frame_buffer frame;
geometry screen_quad;

cubemap cube_map;

free_camera cam;

shadow_map shadow;

spot_light spot;

double cursor_x = 0.0;
double cursor_y = 0.0;
GLFWwindow* window;


bool initialise() {
	// *********************************
	// Set input mode - hide the cursor
	glfwSetInputMode(renderer::get_window(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	// Capture initial mouse position

	glfwGetCursorPos(renderer::get_window(), &cursor_x, &cursor_y);
	// *********************************
	return true;
}

bool load_content() {
	// Create frame buffer - use screen width and height
	frame = frame_buffer(renderer::get_screen_width(), renderer::get_screen_height());

	// Create screen quad
	vector<vec3> positions{ vec3(-1.0f, -1.0f, 0.0f), vec3(1.0f, -1.0f, 0.0f), vec3(-1.0f, 1.0f, 0.0f),
						vec3(1.0f, 1.0f, 0.0f) };
	vector<vec2> tex_coords{ vec2(0.0, 0.0), vec2(1.0f, 0.0f), vec2(0.0f, 1.0f), vec2(1.0f, 1.0f) };
	screen_quad.add_buffer(positions, BUFFER_INDEXES::POSITION_BUFFER);
	screen_quad.add_buffer(tex_coords, BUFFER_INDEXES::TEXTURE_COORDS_0);
	screen_quad.set_type(GL_TRIANGLE_STRIP);
	// *********************************

	//create shadow map
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

	// Create box geometry for skybox
	skybox = mesh(geometry_builder::create_box());
	// Scale box by 100
	skybox.get_transform().scale = vec3(100.0f, 100.0f, 100.0f);

	// Load the cubemap
	array<string, 6> filenames = {
		  "textures/mp_bluemoon/bluemoon_ft.tga",
		  "textures/mp_bluemoon/bluemoon_bk.tga",
		  "textures/mp_bluemoon/bluemoon_up.tga",
		  "textures/mp_bluemoon/bluemoon_dn.tga",
		  "textures/mp_bluemoon/bluemoon_rt.tga",
		  "textures/mp_bluemoon/bluemoon_lf.tga" };

	// Create cube_map
	cube_map = cubemap(filenames);

	// Create scene
	meshes["plane"] = mesh(geometry_builder::create_plane());

	meshes["teapot"].get_transform().scale = vec3(3.0f, 3.0f, 3.0f);
	meshes["teapot"] = mesh(geometry("models/teapot.obj"));
	meshes["teapot"].get_transform().scale = vec3(0.1f, 0.1f, 0.1f);
	meshes["teapot"].get_transform().translate(vec3(-40.0f, 0.0f, 20.0f));

	meshes["crow"] = mesh(geometry("models/Crow/CROW.OBJ"));
	meshes["crow"].get_transform().scale = vec3(60.0f);
	meshes["crow"].get_transform().translate(vec3(0.0f, 0.0f, 0.0f));

	meshes["sword"] = mesh(geometry("models/claymore.obj"));
	meshes["sword"].get_transform().scale = vec3(0.3f);
	meshes["sword"].get_transform().translate(vec3(-35.0f, 10.0f, 1.8f));
	// meshes["sword"].get_transform().orientation = vec3(half_pi<float>(), 0.0f, 0.0f);

	meshes["hand"] = mesh(geometry("models/hand.obj"));
	meshes["hand"].get_transform().translate(vec3(0.0, 0.0f, 30.0f));
	meshes["hand"].get_transform().orientation = vec3(-half_pi<float>(), quarter_pi<float>(), 0);

	reflective_meshes["light"] = mesh(geometry_builder::create_sphere(20, 20));

	reflective_meshes["box"] = mesh(geometry_builder::create_box());
	reflective_meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	reflective_meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));

	reflective_meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	reflective_meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	reflective_meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));

	reflective_meshes["disk"] = mesh(geometry_builder::create_disk(20));
	reflective_meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
	reflective_meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
	reflective_meshes["disk"].get_transform().orientation = vec3(half_pi<float>(), 0.0f, 0.0f);

	reflective_meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
	reflective_meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	reflective_meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));

	reflective_meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
	reflective_meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
	reflective_meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));

	reflective_meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));
	reflective_meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	reflective_meshes["torus"].get_transform().orientation = vec3(half_pi<float>(), 0.0f, 0.0f);



	// Load textures
	textures["plane"] = texture("textures/marble.jpg");
	textures["sword"] = texture("textures/pink.jpg");
	textures["crow"] = texture("textures/pink.jpg");
	textures["teapot"] = texture("textures/blue.jpg");
	textures["hand"] = texture("textures/purple.jpg");


	//Set mesh materials
	meshes["plane"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["plane"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	meshes["sword"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["sword"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	meshes["crow"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["crow"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	meshes["teapot"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["teapot"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	meshes["hand"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["hand"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Main eff
	eff.add_shader("001_Coursework/shader.vert", GL_VERTEX_SHADER);
	vector<string> frag_shaders{ "001_Coursework/shader.frag", "shaders/part_spot.frag", "shaders/part_shadow.frag","001_Coursework/toon.frag" };
	eff.add_shader(frag_shaders, GL_FRAGMENT_SHADER);
	eff.build();

	//Reflective eff
	reflective_eff.add_shader("001_Coursework/reflective_eff.vert", GL_VERTEX_SHADER);
	reflective_eff.add_shader("001_Coursework/reflective_eff.frag", GL_FRAGMENT_SHADER);
	reflective_eff.build();

	// Skybox effect
	sky_eff.add_shader("shaders/skybox.vert", GL_VERTEX_SHADER);
	sky_eff.add_shader("shaders/skybox.frag", GL_FRAGMENT_SHADER);
	sky_eff.build();

	//Shadow eff
	shadow_eff.add_shader("001_Coursework/spot.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("001_Coursework/spot.frag", GL_FRAGMENT_SHADER);
	shadow_eff.build();

	//Blur eff
	blur_eff.add_shader("001_Coursework/simple_texture.vert", GL_VERTEX_SHADER);
	blur_eff.add_shader("001_Coursework/blur.frag", GL_FRAGMENT_SHADER);
	blur_eff.build();

	// Set spot properties
	spot.set_position(vec3(0.0f, 30.0f, 20.0f));
	spot.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	// spot.set_direction(normalize(vec3(-1.0f, 0.0f, 0.0f)));
	spot.set_direction(normalize(vec3(-1.0f, -1.0f, 0.0f)));
	spot.set_range(1000.0f);
	spot.set_power(100.0f);

	reflective_meshes["light"].get_transform().position = spot.get_position();
	reflective_meshes["light"].get_transform().orientation = normalize(vec3(spot.get_direction()));

	// Set camera properties
	cam.set_position(vec3(-10.0f, 10.0f, 35.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	skybox.get_transform().position = cam.get_position();

	return true;
}

bool update(float delta_time) {

	float speed = 30.0f;

	double current_x;
	double current_y;

	static const float screenHeight = static_cast<float>(renderer::get_screen_height());
	static const float screenWidth = static_cast<float>(renderer::get_screen_height());

	static const double ratio_width = quarter_pi<float>() / screenWidth;
	static const double ratio_height = (quarter_pi<float>() * (screenHeight / screenWidth)) / screenHeight;

	// Get the current cursor position
	glfwGetCursorPos(renderer::get_window(), &current_x, &current_y);

	// Calculate delta of cursor positions from last frame
	double delta_x = current_x - cursor_x;
	double delta_y = current_y - cursor_y;

	// Multiply deltas by ratios - gets actual change in orientation
	delta_x = delta_x * ratio_width;
	delta_y = delta_y * ratio_height;

	// Rotate the sphere
	reflective_meshes["sphere"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

	//Move light
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT_SHIFT))
	{
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
			spot.move(vec3(0, delta_time * speed, 0));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN)) {
			spot.move(vec3(0, -delta_time * speed, 0));
		}
	}
	else
	{
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_UP)) {
			spot.move(vec3(0, 0, -delta_time * speed));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_DOWN)) {
			spot.move(vec3(0, 0, delta_time * speed));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_LEFT)) {
			spot.move(vec3(-delta_time * speed, 0, 0));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_RIGHT)) {
			spot.move(vec3(delta_time * speed, 0, 0));
		}

	}

	//Move camera
	cam.rotate(delta_x, -delta_y);
	// Use keyboard to move the camera - WSAD

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		cam.move(vec3(0, 0, delta_time * speed));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		cam.move(vec3(0, 0, -delta_time * speed));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		cam.move(vec3(-delta_time * speed, 0, 0));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		cam.move(vec3(delta_time*speed, 0, 0));
	}
	if (glfwGetKey(renderer::get_window(), 'X') == GLFW_PRESS)
		shadow.buffer->save("test.png");

	// Update the camera
	cam.update(delta_time);

	// Update cursor pos
	cursor_x = current_x;
	cursor_y = current_y;

	// Update the shadow map light_position from the spot light
	shadow.light_position = spot.get_position();

	// do the same for light_dir property
	shadow.light_dir = spot.get_direction();

	//Move light mesh
	reflective_meshes["light"].get_transform().position = (spot.get_position());

	//Keep scene in centre of skybox
	skybox.get_transform().position = cam.get_position();

	return true;
}

bool render() {
	// // Set render target to frame buffer
	// renderer::set_render_target(frame);
	//
	// // Clear frame
	// glClear(GL_FRAMEBUFFER_BARRIER_BIT);

	// // *************************
	// Skybox render
	// Disable depth test,depth mask,face culling
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glDisable(GL_CULL_FACE);
	
	// Bind skybox effect
	renderer::bind(sky_eff);
	
	// Calculate MVP for the skybox
	auto M = skybox.get_transform().get_transform_matrix();
	auto V = cam.get_view();
	auto P = cam.get_projection();
	auto MVP = P * V * M;
	
	// Set MVP matrix uniform
	glUniformMatrix4fv(sky_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
	
	// Bind cubemap to TU 0
	renderer::bind(cube_map, 0);
	
	// Set cubemap uniform
	glUniform1i(sky_eff.get_uniform_location("cubemap"), 0);
	
	// Render skybox
	renderer::render(skybox);
	
	// Enable depth test,depth mask,face culling
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_CULL_FACE);


	// Shadow render
// Set render target to shadow map
	renderer::set_render_target(shadow);
	// Clear depth buffer bit
	glClear(GL_DEPTH_BUFFER_BIT);
	// Set face cull mode to front
	glCullFace(GL_FRONT);
	// *********************************

	mat4 LightProjectionMat = perspective<float>(90.f, renderer::get_screen_aspect(), 0.1f, 1000.f);

	// Bind shader
	renderer::bind(shadow_eff);
	// Render reflective mesh shadows
	for (auto &e : reflective_meshes) {
		auto m = e.second;
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		// *********************************
		// View matrix taken from shadow map
		auto V = shadow.get_view();

		// *********************************
		auto MVP = LightProjectionMat * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Render mesh
		renderer::render(m);
	}

	// Render non-reflective mesh shadows
	for (auto &e : meshes) {
		auto m = e.second;
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		// *********************************
		// View matrix taken from shadow map
		auto V = shadow.get_view();

		// *********************************
		auto MVP = LightProjectionMat * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
		// Pointer to matrix data
		// Render mesh
		renderer::render(m);
	}
	// *********************************
	// Set render target back to the screen
	renderer::set_render_target();

	// Set face cull mode to back
	glCullFace(GL_BACK);
	// *********************************

	// Render reflective meshes
	renderer::bind(reflective_eff);
	// Render reflective meshes
	for (auto &e : reflective_meshes) {
		auto m = e.second;
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto V = cam.get_view();
		auto P = cam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
		// *********************************
		// Set lightMVP uniform, using:
		 //Model matrix from m
		auto lM = m.get_transform().get_transform_matrix();
		// viewmatrix from the shadow map
		auto lV = shadow.get_view();

		// Multiply together with LightProjectionMat
		auto lMVP = LightProjectionMat * lV * lM;

		// Set uniform
		glUniformMatrix4fv(eff.get_uniform_location("lMVP"), // Location of uniform
			1,                                    // Number of values - 1 mat4
			GL_FALSE,                             // Transpose the matrix?
			value_ptr(lMVP));                      // Pointer to matrix data

		// Set eye_pos value
		vec3 eye_pos(cam.get_position());
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(eye_pos));

		// Bind cubemap to TU 0
		renderer::bind(cube_map, 0);

		// Set cubemap uniform
		glUniform1i(eff.get_uniform_location("cube_map"), 0);


		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind spot light
		renderer::bind(spot, "spot");

		// Bind cubemap to TU 0
		renderer::bind(cube_map, 0);

		// Set cubemap uniform
		glUniform1i(eff.get_uniform_location("cube_map"), 0);


		// Bind shadow map texture - use texture unit 1
		renderer::bind(shadow.buffer->get_depth(), 1);

		// Set the shadow_map uniform
		glUniform1i(eff.get_uniform_location("shadow"), 1);

		// Render mesh
		renderer::render(m);

		// *********************************
	}

	// Bind shader
	renderer::bind(eff);
	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto V = cam.get_view();
		auto P = cam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(m.get_transform().get_normal_matrix()));
		// *********************************
		// Set lightMVP uniform, using:
		 //Model matrix from m
		auto lM = m.get_transform().get_transform_matrix();
		// viewmatrix from the shadow map
		auto lV = shadow.get_view();

		// Multiply together with LightProjectionMat
		auto lMVP = LightProjectionMat * lV * lM;

		// Set uniform
		glUniformMatrix4fv(eff.get_uniform_location("lMVP"), // Location of uniform
			1,                                    // Number of values - 1 mat4
			GL_FALSE,                             // Transpose the matrix?
			value_ptr(lMVP));                      // Pointer to matrix data

		// Set eye_pos value
		vec3 eye_pos(cam.get_position());
		glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(eye_pos));

		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind spot light
		renderer::bind(spot, "spot");

		// Bind and set textures
		string mesh_name = e.first;
		texture tex = textures[mesh_name];

		// Bind texture
		renderer::bind(tex, 0);

		// Set tex uniform
		glUniform1i(eff.get_uniform_location("tex"), 0);

		// Bind shadow map texture - use texture unit 1
		renderer::bind(shadow.buffer->get_depth(), 1);

		// Set the shadow_map uniform
		glUniform1i(eff.get_uniform_location("shadow"), 1);

		// Render mesh
		renderer::render(m);

		// *********************************
	}


	// *********************************
		// Set render target back to the screen
	// 	renderer::set_render_target();
	//
	// // bind the blur effect
	// renderer::bind(blur_eff);
	//
	// // MVP is now the identity matrix
	// mat4 identity(1.0f);
	// // MVP = identity;
	//
	// // Set MVP matrix uniform
	// glUniformMatrix4fv(blur_eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(identity));
	//
	// // Bind texture from frame buffer
	// renderer::bind(frame.get_frame(), 0);
	//
	// // Set the tex uniform
	// glUniform1i(blur_eff.get_uniform_location("tex"), 0);
	//
	// // Set inverse width Uniform
	// float inverse_width = 1.0f / renderer::get_screen_width();
	// glUniform1f(blur_eff.get_uniform_location("inverse_width"), inverse_width);
	//
	// // Set inverse height Uniform
	// float inverse_height = 1.0f / renderer::get_screen_height();
	// glUniform1f(blur_eff.get_uniform_location("inverse_height"), inverse_height);
	//
	// // Render the screen quad
	// renderer::render(screen_quad);
	// *********************************

	return true;
}

void main() {
	// Create application
	app application("001_Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}