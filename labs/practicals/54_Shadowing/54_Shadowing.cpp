#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
effect main_eff;
effect shadow_eff;
texture tex;

free_camera cam;
spot_light spot;
shadow_map shadow;

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
	// *********************************
	// Create shadow map- use screen size
	shadow = shadow_map(renderer::get_screen_width(), renderer::get_screen_height());

	// Create plane mesh
	meshes["plane"] = mesh(geometry_builder::create_plane());

	// Create "teapot" mesh by loading in models/teapot.obj
	meshes["teapot"] = mesh(geometry("models/teapot.obj"));

	// Translate Teapot(0,4,0)
	meshes["teapot"].get_transform().translate(vec3(0.0f, 0.0f, 0.0f));

	// Scale the teapot - (0.1, 0.1, 0.1)
	meshes["teapot"].get_transform().scale = vec3(0.1f, 0.1f, 0.1f);

	// *********************************

	// Load texture
	tex = texture("textures/checked.gif");

	// ***********************
	// Set materials
	// - all emissive is black
	// - all specular is white
	// - all shininess is 25
	// ***********************
	// White plane
	meshes["plane"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["plane"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["plane"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["plane"].get_material().set_shininess(25.0f);
	// Red teapot
	meshes["teapot"].get_material().set_emissive(vec4(0.0f, 0.0f, 0.0f, 1.0f));
	meshes["teapot"].get_material().set_diffuse(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	meshes["teapot"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["teapot"].get_material().set_shininess(25.0f);

	meshes["box"] = mesh(geometry_builder::create_box());
	meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
	meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	meshes["disk"] = mesh(geometry_builder::create_disk(20));
	meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
	meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));

	meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	// meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));

	meshes["tetra"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
	// meshes["tetra"].get_transform().translate(vec3(-30.0f, 10.0f, -10.0f));

	meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	// meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));

	meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
	meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
	meshes["disk"].get_transform().orientation = vec3(half_pi<float>(), 0.0f, 0.0f);

	meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));

	meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
	meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));

	meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().orientation = vec3(half_pi<float>(), 0.0f, 0.0f);
	// *******************
	// Set spot properties
	// *******************
	// Pos (20, 30, 0), White
	// Direction (-1, -1, 0) normalized
	// 50 range, 10 power
	spot.set_position(vec3(-25.0f, 5.0f, -10.0f));

	// Light colour white
	spot.set_light_colour(vec4(1.0f, 1.0f, 1.0f, 1.0f));

	// Light direction to forward and down (normalized)
	spot.set_direction(normalize(vec3(1.0f, -1.0f, 1.0f)));

	// Set range to 20
	spot.set_range(50.0f);

	// Set power to 1
	spot.set_power(1.0f);
	spot.set_constant_attenuation(0.05f);
	spot.set_linear_attenuation(0.01f);
	spot.set_linear_attenuation(0.01f);

	// Load in shaders
	main_eff.add_shader("54_Shadowing/shader.vert", GL_VERTEX_SHADER);
	vector<string> frag_shaders{ "54_Shadowing/shader.frag", "shaders/part_spot.frag", "shaders/part_shadow.frag" };
	main_eff.add_shader(frag_shaders, GL_FRAGMENT_SHADER);

	shadow_eff.add_shader("50_Spot_Light/spot.vert", GL_VERTEX_SHADER);
	shadow_eff.add_shader("50_Spot_Light/spot.frag", GL_FRAGMENT_SHADER);

	// Build effects
	main_eff.build();
	shadow_eff.build();

	// Set camera properties
	cam.set_position(vec3(0.0f, 50.0f, -75.0f));
	cam.set_target(vec3(0.0f, 0.0f, 0.0f));
	cam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);
	return true;
}

bool update(float delta_time) {
	// Rotate the teapot
	// meshes["teapot"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

	float speed = 10.0f;

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

	//free camera 

		// Rotate cameras by delta
		// delta_y - x-axis rotation
		// delta_x - y-axis rotation
	cam.rotate(delta_x*2, -delta_y*2 );
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



	cam.update(delta_time);


	cursor_x = current_x;
	cursor_y = current_y;
	// *********************************
	// Update the shadow map light_position from the spot light
	shadow.light_position = spot.get_position();

	// do the same for light_dir property
	shadow.light_dir = spot.get_direction();

	// *********************************

	// Press s to save
	if (glfwGetKey(renderer::get_window(), 'X') == GLFW_PRESS)
		shadow.buffer->save("test.png");

	return true;
}

bool render() {
	// *********************************
	// Set render target to shadow map
	renderer::set_render_target(shadow);

	// Clear depth buffer bit
	glClear(GL_DEPTH_BUFFER_BIT);

	// Set face cull mode to front
	// glCullFace(GL_FRONT);

	// *********************************

	// We could just use the Camera's projection, 
	// but that has a narrower FoV than the cone of the spot light, so we would get clipping.
	// so we have yo create a new Proj Mat with a field of view of 90.
	mat4 LightProjectionMat = perspective<float>(90.f, renderer::get_screen_aspect(), 0.1f, 1000.f);

	// Bind shader
	renderer::bind(shadow_eff);
	// Render meshes
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
		glUniformMatrix4fv(shadow_eff.get_uniform_location("MVP"), // Location of uniform
			1,                                      // Number of values - 1 mat4
			GL_FALSE,                               // Transpose the matrix?
			value_ptr(MVP));                        // Pointer to matrix data
// Render mesh
		renderer::render(m);
	}
	// *********************************
	// Set render target back to the screen
	renderer::set_render_target();

	// Set face cull mode to back
	glCullFace(GL_BACK);

	// *********************************

	// Bind shader
	renderer::bind(main_eff);

	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Create MVP matrix
		auto M = m.get_transform().get_transform_matrix();
		auto V = cam.get_view();
		auto P = cam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("MVP"), // Location of uniform
			1,                                    // Number of values - 1 mat4
			GL_FALSE,                             // Transpose the matrix?
			value_ptr(MVP));                      // Pointer to matrix data
// Set M matrix uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));
		// Set N matrix uniform
		glUniformMatrix3fv(main_eff.get_uniform_location("N"), 1, GL_FALSE,
			value_ptr(m.get_transform().get_normal_matrix()));
		// *********************************
		// Set lightMVP uniform, using:
		 //Model matrix from m
		auto lM = m.get_transform().get_transform_matrix();

		// viewmatrix from the shadow map
		auto lV = shadow.get_view();

		// Multiply together with LightProjectionMat
		auto lMVP = LightProjectionMat * lV * lM;

		// Set uniform
		glUniformMatrix4fv(main_eff.get_uniform_location("lMVP"), // Location of uniform
			1,                                    // Number of values - 1 mat4
			GL_FALSE,                             // Transpose the matrix?
			value_ptr(lMVP));                      // Pointer to matrix data
		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind spot light
		renderer::bind(spot, "spot");

		// Bind texture
		renderer::bind(tex, 0);

		// Set tex uniform
		glUniform1i(main_eff.get_uniform_location("tex"), 0);

		// Set eye position
		vec3 eye_pos(cam.get_position());
		glUniform3fv(main_eff.get_uniform_location("eye_pos"), 1, value_ptr(eye_pos));

		// Bind shadow map texture - use texture unit 1
		auto shadow_map = shadow.buffer->get_depth();
		renderer::bind(shadow_map, 1);

		// Set the shadow_map uniform
		glUniform1i(main_eff.get_uniform_location("shadow_map"), 1);

		// Render mesh
		renderer::render(m);

		// *********************************
	}

	return true;
}

void main() {
	// Create application
	app application("54_Shadowing");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}