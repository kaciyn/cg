#include <glm\glm.hpp>
#include <graphics_framework.h>
#define _USE_MATH_DEFINES

#include <cmath>
#include <iostream>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
map<string, texture> textures;
effect eff;

target_camera targetcam;
free_camera freecam;
chase_camera chasecam;

// directional_light light;
vector<point_light> points(4);
vector<spot_light> spots(5);

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
	// Create plane mesh
	meshes["plane"] = mesh(geometry_builder::create_plane());

	// Create scene
	meshes["box"] = mesh(geometry_builder::create_box());
	meshes["tetra"] = mesh(geometry_builder::create_tetrahedron());
	meshes["pyramid"] = mesh(geometry_builder::create_pyramid());
	meshes["disk"] = mesh(geometry_builder::create_disk(20));
	meshes["cylinder"] = mesh(geometry_builder::create_cylinder(20, 20));
	meshes["sphere"] = mesh(geometry_builder::create_sphere(20, 20));
	meshes["torus"] = mesh(geometry_builder::create_torus(20, 20, 1.0f, 5.0f));

	// Transform objects
	meshes["box"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));

	meshes["tetra"].get_transform().scale = vec3(4.0f, 4.0f, 4.0f);
	meshes["tetra"].get_transform().translate(vec3(-30.0f, 10.0f, -10.0f));

	meshes["pyramid"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));

	meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);
	meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
	meshes["disk"].get_transform().orientation = vec3(half_pi<float>(), 0.0f, 0.0f);

	meshes["cylinder"].get_transform().scale = vec3(5.0f, 5.0f, 5.0f);
	meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));

	meshes["sphere"].get_transform().scale = vec3(2.5f, 2.5f, 2.5f);
	meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));

	meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().orientation = vec3(half_pi<float>(), 0.0f, 0.0f);

	// Create mesh to chase
	// meshes["chaser"] = mesh(geometry_builder::create_box());
	meshes["chaser"] = mesh(geometry("models/Crow/CROW.OBJ"));
	meshes["chaser"].get_transform().scale = vec3(15.0f);
	meshes["chaser"].get_transform().translate(vec3(0.0f, 0.0f, 0.0f));

	// meshes["sword"] = mesh(geometry_builder::create_box());
	meshes["sword"] = mesh(geometry("models/claymore.obj"));
	meshes["sword"].get_transform().scale = vec3(0.005f);
	meshes["sword"].get_transform().translate(vec3(-0.5f, 0.21f, 0.12f));
	meshes["sword"].get_transform().orientation = vec3(half_pi<float>(), pi<float>(), half_pi<float>());

	// Load textures
	textures["plane"] = texture("textures/blackmarble.jpg");
	textures["box"] = texture("textures/checked.gif");
	textures["tetra"] = texture("textures/confirmed.png");
	textures["pyramid"] = texture("textures/sand.jpg");
	textures["disk"] = texture("textures/stone.jpg");
	textures["cylinder"] = texture("textures/snow.jpg");
	textures["sphere"] = texture("textures/heightmap.jpg");
	textures["torus"] = texture("textures/grass.jpg");
	textures["sword"] = texture("textures/sword.jpg");
	textures["chaser"] = texture("textures/black.jpg");

	//for some reason the diffuse nor the specular lighting has been happening so i've set the emissive so nonzero so the textures aren't all Black
	//Set mesh materials
	meshes["plane"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["plane"].get_material().set_emissive(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	meshes["plane"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["plane"].get_material().set_shininess(0.0f);

	meshes["box"].get_material().set_diffuse(vec4(1.0f, 1.0f, 0.0f, 1.0f));
	meshes["box"].get_material().set_emissive(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	meshes["box"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["box"].get_material().set_shininess(5.0f);

	meshes["tetra"].get_material().set_emissive(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	meshes["tetra"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["tetra"].get_material().set_shininess(13.0f);

	meshes["pyramid"].get_material().set_diffuse(vec4(0.0f, 1.0f, 1.0f, 1.0f));
	meshes["pyramid"].get_material().set_emissive(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	meshes["pyramid"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["pyramid"].get_material().set_shininess(10.0f);

	meshes["disk"].get_material().set_diffuse(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	meshes["disk"].get_material().set_emissive(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	meshes["disk"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["disk"].get_material().set_shininess(5.0f);

	meshes["cylinder"].get_material().set_diffuse(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["cylinder"].get_material().set_emissive(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	meshes["cylinder"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["cylinder"].get_material().set_shininess(15.0f);

	meshes["sphere"].get_material().set_diffuse(vec4(1.0f, 0.0f, 1.0f, 1.0f));
	meshes["sphere"].get_material().set_emissive(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	meshes["sphere"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["sphere"].get_material().set_shininess(10.0f);

	meshes["torus"].get_material().set_diffuse(vec4(0.5f, 1.0f, 0.0f, 1.0f));
	meshes["torus"].get_material().set_emissive(vec4(0.5f, 0.5f, 0.5f, 1.0f));
	meshes["torus"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["torus"].get_material().set_shininess(0.0f);

	meshes["sword"].get_material().set_emissive(vec4(0.5f, 0.8f, 0.0f, 1.0f));
	meshes["sword"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["sword"].get_material().set_shininess(10.0f);

	meshes["chaser"].get_material().set_emissive(vec4(0.6f, 0.6f, 0.5f, 1.0f));
	meshes["chaser"].get_material().set_specular(vec4(1.0f, 1.0f, 1.0f, 1.0f));
	meshes["chaser"].get_material().set_shininess(25.0f);

	// Set lighting values
// *********************************
// Point 0, Position (-25, 5, -15)
// Red, 20 range
	points[0].set_position(vec3((-25.0f, 5.0f, -15.0f)));
	points[0].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	points[0].set_range(20.0f);

	// Point 1, Position (-25, 5, -35)
	// Red,20 range
	points[1].set_position(vec3((-25.0f, 5.0f, -35.0f)));
	points[1].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	points[1].set_range(20.0f);

	// Point 2,Position (-10, 5, -15)
	// Red,20 range
	points[2].set_position(vec3((-10.0f, 5.0f, -15.0f)));
	points[2].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	points[2].set_range(20.0f);

	// Point 3,Position (-10, 5, -35)
// Red,20 range
	points[3].set_position(vec3((-10.0f, 5.0f, -35.0f)));
	points[3].set_light_colour(vec4(1.0f, 0.0f, 0.0f, 1.0f));
	points[3].set_range(20.0f);

	// Spot 0, Position (-25, 10, -15)
	// Green, Direction (1, -1, -1) normalized
	// 20 range,0.5 power
	spots[0].set_position(vec3((-25.0f, 5.0f, -15.0f)));
	spots[0].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	spots[0].set_range(20.0f);
	spots[0].set_direction(normalize(vec3(1.0f, -1.0f, -1.0f)));
	spots[0].set_power(0.5f);

	// Spot 1,Position (-25, 10, -35)
	// Green,Direction (1, -1, 1) normalized
	// 20 range,0.5 power
	spots[1].set_position(vec3((-25.0f, 5.0f, -35.0f)));
	spots[1].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	spots[1].set_range(20.0f);
	spots[1].set_direction(normalize(vec3(1.0f, -1.0f, -1.0f)));
	spots[1].set_power(0.5f);

	// Spot 2,Position (-10, 10, -15)
// Green,Direction (-1, -1, -1) normalized
// 20 range,0.5 power
	spots[2].set_position(vec3((-10.0f, 10.0f, -15.0f)));
	spots[2].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	spots[2].set_range(20.0f);
	spots[2].set_direction(normalize(vec3(1.0f, -1.0f, -1.0f)));
	spots[2].set_power(0.5f);

	// Spot 3,Position (-10, 10, -35)
// Green,Direction (-1, -1, 1) normalized
// 20 range,0.5 power
	spots[3].set_position(vec3((-10.0f, 10.0f, -35.0f)));
	spots[3].set_light_colour(vec4(0.0f, 1.0f, 0.0f, 1.0f));
	spots[3].set_range(20.0f);
	spots[3].set_direction(normalize(vec3(1.0f, -1.0f, -1.0f)));
	spots[3].set_power(0.5f);

	// Spot 4,Position (-17.5, 15, -25)
	// Blue,Direction (0, -1, 0)
	// 30 range,1.0 power
	spots[4].set_position(vec3((-25.0f, 5.0f, -15.0f)));
	spots[4].set_light_colour(vec4(0.0f, 0.0f, 1.0f, 1.0f));
	spots[4].set_range(30.0f);
	spots[4].set_direction(normalize(vec3(0.0f, -1.0f, 0.0f)));
	spots[4].set_power(1.0f);

	// Load in shaders
	eff.add_shader("000_Coursework/texture.vert", GL_VERTEX_SHADER);
	eff.add_shader("000_Coursework/texture.frag", GL_FRAGMENT_SHADER);

	// Build effect
	eff.build();

	// target camera properties
	targetcam.set_position(vec3(50.0f, 10.0f, 50.0f));
	targetcam.set_target(vec3(0.0f, 0.0f, 0.0f));
	targetcam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	// free camera properties
	freecam.set_position(vec3(0.0f, 10.0f, 0.0f));
	freecam.set_target(vec3(0.0f, 0.0f, 0.0f));
	freecam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	// chase camera properties
	chasecam.set_pos_offset(vec3(0.0f, 10.0f, 15.0f));
	chasecam.set_springiness(0.4f);
	chasecam.move(meshes["chaser"].get_transform().position, eulerAngles(meshes["chaser"].get_transform().orientation));
	chasecam.set_projection(quarter_pi<float>(), renderer::get_screen_aspect(), 0.1f, 1000.0f);

	return true;
}

bool update(float delta_time) {

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

	// Rotate the sphere
	meshes["sphere"].get_transform().rotate(vec3(0.0f, half_pi<float>(), 0.0f) * delta_time);

	// The target object
	static mesh &target_mesh = meshes["chaser"];

	// Use keyboard to rotate target_mesh - QE rotate on y-axis
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_Q)) {
		target_mesh.get_transform().rotate(vec3(0.0f, -pi<float>() * delta_time, 0.0f));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_E)) {
		target_mesh.get_transform().rotate(vec3(0.0f, pi<float>() * delta_time, 0.0f));
	}

	if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
		target_mesh.get_transform().translate(vec3(0, 0, -delta_time * speed));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
		target_mesh.get_transform().translate(vec3(0, 0, delta_time * speed));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
		target_mesh.get_transform().translate(vec3(-delta_time * speed, 0, 0));
	}
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
		target_mesh.get_transform().translate(vec3(delta_time*speed, 0, 0));
	}

	//free camera 
	if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
	{
		// Rotate cameras by delta
		// delta_y - x-axis rotation
		// delta_x - y-axis rotation
		freecam.rotate(delta_x, -delta_y);
		// Use keyboard to move the camera - WSAD

		if (glfwGetKey(renderer::get_window(), GLFW_KEY_W)) {
			freecam.move(vec3(0, 0, delta_time * speed));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_S)) {
			freecam.move(vec3(0, 0, -delta_time * speed));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_A)) {
			freecam.move(vec3(-delta_time * speed, 0, 0));
		}
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_D)) {
			freecam.move(vec3(delta_time*speed, 0, 0));
		}
	}
	//chase camera 
	else {

		// Rotate cameras by delta
		chasecam.rotate(vec3(-delta_y, -delta_x, 0));

		// Move camera - update target position and rotation
		chasecam.move(meshes["chaser"].get_transform().position, eulerAngles(meshes["chaser"].get_transform().orientation));
	}

	// Update the cameras
	freecam.update(delta_time);
	targetcam.update(delta_time);
	chasecam.update(delta_time);

	// Update cursor pos
	cursor_x = current_x;
	cursor_y = current_y;

	return true;
}

bool render() {
	// Render meshes
	for (auto &e : meshes) {
		auto m = e.second;
		// Bind effect
		renderer::bind(eff);
		// Create MVP matrix

		auto M = m.get_transform().get_transform_matrix();

		//sword transform
		if (e.first == "sword")
		{
			M = meshes["chaser"].get_transform().get_transform_matrix() * M;
		}
		//free camera
		if (glfwGetKey(renderer::get_window(), GLFW_KEY_1))
		{
			auto V = freecam.get_view();
			auto P = freecam.get_projection();
			auto MVP = P * V * M;

			glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
			// Set eye position - Get this from active camera
			vec3 eye_pos(freecam.get_position());
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(eye_pos));
		}
		//target camera
		else if (glfwGetKey(renderer::get_window(), GLFW_KEY_2))
		{
			auto V = targetcam.get_view();
			auto P = targetcam.get_projection();
			auto MVP = P * V * M;

			glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));
			// Set eye position - Get this from active camera
			vec3 eye_pos(targetcam.get_position());
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(eye_pos));
		}
		//chase camera
		else {
			auto V = chasecam.get_view();
			auto P = chasecam.get_projection();
			auto MVP = P * V * M;
			glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

			// Set eye position - Get this from active camera
			vec3 eye_pos(chasecam.get_position());
			glUniform3fv(eff.get_uniform_location("eye_pos"), 1, value_ptr(eye_pos));
		}

		// Set M matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("M"), 1, GL_FALSE, value_ptr(M));

		// Set N matrix uniform - remember - 3x3 matrix
		mat3 N = m.get_transform().get_normal_matrix();
		glUniformMatrix3fv(eff.get_uniform_location("N"), 1, GL_FALSE, value_ptr(N));

		// Bind material
		renderer::bind(m.get_material(), "mat");

		// Bind point lights
		vector <point_light> points;
		renderer::bind(points, "points");

		// Bind spot lights
		vector <spot_light> spots;
		renderer::bind(spots, "spots");

		// Bind and set textures
		string mesh_name = e.first;
		texture tex = textures[mesh_name];

		renderer::bind(tex, 0);
		glUniform1i(eff.get_uniform_location("tex"), 0);

		// Render mesh
		renderer::render(m);
	}

	return true;
}

void main() {
	// Create application
	app application("000_Coursework");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_initialise(initialise);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}