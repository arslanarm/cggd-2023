#include "rasterizer_renderer.h"
#define M_PI           3.14159265358979323846  /* pi */

#include "utils/resource_utils.h"


void cg::renderer::rasterization_renderer::init()
{
	rasterizer = std::make_shared<cg::renderer::rasterizer<cg::vertex, cg::unsigned_color>>();
	rasterizer->set_viewport(settings->width, settings->height);
	render_target = std::make_shared<cg::resource<unsigned_color>>(
	settings->width, settings->height
	);
	final_render_target = std::make_shared<cg::resource<unsigned_color>>(
			settings->width, settings->height
	);
	depth_buffer = std::make_shared<cg::resource<float>>(
			settings->width, settings->height
	);
	rasterizer->set_render_target(render_target, depth_buffer, final_render_target);
	model = std::make_shared<cg::world::model>();
	model->load_obj(settings->model_path);
	camera = std::make_shared<cg::world::camera>();
	camera->set_height((float) settings->height);
	camera->set_width((float) settings->width);
	camera->set_position(float3 {
			settings->camera_position[0],
			settings->camera_position[1],
			settings->camera_position[2]
	});
	camera->set_phi(settings->camera_phi);
	camera->set_theta(settings->camera_theta);
	camera->set_angle_of_view(settings->camera_angle_of_view);
	camera->set_z_far(settings->camera_z_far);
	camera->set_z_near(settings->camera_z_near);

}
void cg::renderer::rasterization_renderer::render()
{
	auto start = std::chrono::high_resolution_clock::now();
	rasterizer->clear_render_target({0, 0, 0});
	auto stop = std::chrono::high_resolution_clock::now();

	std::chrono::duration<float, std::milli> duration = stop - start;
	std::cout << "Clear took " << duration.count() << "ms\r\n";

	float4x4 matrix = mul(
			camera->get_projection_matrix(),
			camera->get_view_matrix(),
			model->get_world_matrix()
			);

	rasterizer->vertex_shader = [&](float4 vertex, cg::vertex data) {
		auto processed = mul(matrix, vertex);
		return std::pair(processed, data);
	};

	rasterizer->pixel_shader = [](cg::vertex vertex_data, float z) {
		return cg::color {
				vertex_data.ambient_r,
				vertex_data.ambient_g,
				vertex_data.ambient_b
		};
	};
	int std = 5;
	// Blur
	rasterizer->compute_shader = [&](const std::shared_ptr<resource<cg::unsigned_color>>& texture, const size_t i) {
		auto x = i % texture->get_stride(), y = i / texture->get_stride();
		float3 color;
		for (int j = -3/2*std; j <= 3/2*std; j++) {
			for (int k = -3/2*std; k <= 3/2*std; k++) {
				float3 value;
				if (x + j < 0 || x + j >= texture->get_stride() ||
					y + k < 0 || y + k >= texture->get_number_of_elements() / texture->get_stride())
					value = float3 { 0, 0, 0 };
				else
					value = texture->item(x + j, y + k).to_float3();
				float multiplier = exp(-(double)(j*j + k*k)/(double)(2 * std * std)) / (2 * M_PI * std * std);

				value *= multiplier;
				color += value;
			}
		}
		return unsigned_color::from_float3(color);
	};

	start = std::chrono::high_resolution_clock::now();
	for (size_t shape_id = 0; shape_id < model->get_index_buffers().size(); shape_id++) {
		rasterizer->set_vertex_buffer(model->get_vertex_buffers()[shape_id]);
		rasterizer->set_index_buffer(model->get_index_buffers()[shape_id]);
		rasterizer->draw(model->get_index_buffers()[shape_id]->get_number_of_elements(), 0);
	}
	rasterizer->final_draw();
	stop = std::chrono::high_resolution_clock::now();

	duration = stop - start;
	std::cout << "Render took " << duration.count() << "ms\r\n";

	cg::utils::save_resource(*final_render_target, settings->result_path);
}

void cg::renderer::rasterization_renderer::destroy() {}

void cg::renderer::rasterization_renderer::update() {}