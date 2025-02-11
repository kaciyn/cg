#include <glm\glm.hpp>
#include <graphics_framework.h>

using namespace std;
using namespace graphics_framework;
using namespace glm;

map<string, mesh> meshes;
effect eff;
texture tex;
target_camera cam;

bool load_content() {
	// Create plane mesh
	meshes["plane"] = mesh(geometry_builder::create_plane());

	// *********************************
	// box
	meshes["box"] = mesh(geometry_builder::create_box(vec3(5, 5, 5)));
	// Tetrahedron
	meshes["tetrahedron"] = mesh(geometry_builder::create_tetrahedron(vec3(4, 4, 4)));

	// Pyramid
	meshes["pyramid"] = mesh(geometry_builder::create_pyramid(vec3(5, 5, 5)));

	// Disk
	meshes["disk"] = mesh(geometry_builder::create_disk(uint(20), vec3(3, 1, 3)));

	// Cylinder
	meshes["cylinder"] = mesh(geometry_builder::create_cylinder(uint(20), uint(20), vec3(5, 5, 5)));

	// Sphere
	meshes["sphere"] = mesh(geometry_builder::create_sphere(uint(20), uint(20), vec3(2.5, 2.5, 2.5)));

	// Torus
	meshes["torus"] = mesh(geometry_builder::create_torus(uint(20), uint(20), 1, 5));


	// Set the transforms for your meshes here
	// 5x scale, move(-10.0f, 2.5f, -30.0f)
	meshes["box"].get_transform().translate(vec3(-10.0f, 2.5f, -30.0f));
	meshes["box"].get_transform().scale = vec3(5.0f);

	// 4x scale, move(-30.0f, 10.0f, -10.0f)

	meshes["tetrahedron"].get_transform().translate(vec3(-30.0f, 10.0f, -10.0f));
	meshes["tetrahedron"].get_transform().scale = vec3(4.0f);

	// 5x scale, move(-10.0f, 7.5f, -30.0f)

	meshes["pyramid"].get_transform().translate(vec3(-10.0f, 7.5f, -30.0f));
	meshes["pyramid"].get_transform().scale = vec3(5.0f);

	// scale(3.0f, 1.0f, 3.0f), move(-10.0f, 11.5f, -30.0f), 180 rotate X axis
	meshes["disk"].get_transform().translate(vec3(-10.0f, 11.5f, -30.0f));
	meshes["disk"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));
	meshes["disk"].get_transform().scale = vec3(3.0f, 1.0f, 3.0f);

	// 5x scale, move(-25.0f, 2.5f, -25.0f)
	meshes["cylinder"].get_transform().translate(vec3(-25.0f, 2.5f, -25.0f));
	meshes["cylinder"].get_transform().scale = vec3(5.0f);

	// 2.5x scale, move(-25.0f, 10.0f, -25.0f)
	meshes["sphere"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	meshes["sphere"].get_transform().scale = vec3(2.5f);

	// 180 rotate X axis, move(-25.0f, 10.0f, -25.0f)
	meshes["torus"].get_transform().translate(vec3(-25.0f, 10.0f, -25.0f));
	meshes["torus"].get_transform().rotate(vec3(half_pi<float>(), 0.0f, 0.0f));


	// *********************************

	// Load texture
	tex = texture("textures/checked.gif");
	//TODO texture is working here tho??????
	// Load in shaders
	eff.add_shader("27_Texturing_Shader/simple_texture.vert", GL_VERTEX_SHADER);
	eff.add_shader("27_Texturing_Shader/simple_texture.frag", GL_FRAGMENT_SHADER);
	// Build effect
	eff.build();

	// Set camera properties
	cam.set_position(vec3(10.0f, 10.0f, 10.0f));
	cam.set_target(vec3(-100.0f, 0.0f, -100.0f));
	auto aspect = static_cast<float>(renderer::get_screen_width()) / static_cast<float>(renderer::get_screen_height());
	cam.set_projection(quarter_pi<float>(), aspect, 2.414f, 1000.0f);
	return true;
}

bool update(float delta_time) {
	// Update the camera
	cam.update(delta_time);
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
		auto V = cam.get_view();
		auto P = cam.get_projection();
		auto MVP = P * V * M;
		// Set MVP matrix uniform
		glUniformMatrix4fv(eff.get_uniform_location("MVP"), 1, GL_FALSE, value_ptr(MVP));

		// *********************************
		// Bind texture to renderer
		renderer::bind(tex, 0);

		// Set the texture value for the shader here
		glUniform1i(eff.get_uniform_location("tex"), 0);

		// *********************************
		// Render mesh
		renderer::render(m);
	}

	return true;
}

void main() {
	// Create application
	app application("35_Geometry_Builder");
	// Set load content, update and render methods
	application.set_load_content(load_content);
	application.set_update(update);
	application.set_render(render);
	// Run application
	application.run();
}