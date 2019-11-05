#include "igl/opengl/glfw/renderer.h"

#include <GLFW/glfw3.h>
#include <igl/unproject_onto_mesh.h>
#include "igl/look_at.h"
#include <Eigen/Dense>
Renderer::Renderer() : selected_core_index(0),
next_core_id(2)
{
	core_list.emplace_back(igl::opengl::ViewerCore());
	core_list.front().id = 1;
	// C-style callbacks
	callback_init = nullptr;
	callback_pre_draw = nullptr;
	callback_post_draw = nullptr;
	callback_mouse_down = nullptr;
	callback_mouse_up = nullptr;
	callback_mouse_move = nullptr;
	callback_mouse_scroll = nullptr;
	callback_key_down = nullptr;
	callback_key_up = nullptr;

	callback_init_data = nullptr;
	callback_pre_draw_data = nullptr;
	callback_post_draw_data = nullptr;
	callback_mouse_down_data = nullptr;
	callback_mouse_up_data = nullptr;
	callback_mouse_move_data = nullptr;
	callback_mouse_scroll_data = nullptr;
	callback_key_down_data = nullptr;
	callback_key_up_data = nullptr;
	highdpi = 1;

	xold = 0;
	yold = 0;

}

IGL_INLINE void Renderer::draw( GLFWwindow* window)
{
	using namespace std;
	using namespace Eigen;

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	int width_window, height_window;
	glfwGetWindowSize(window, &width_window, &height_window);
	
	auto highdpi_tmp = (width_window == 0 || width == 0) ? highdpi : (width / width_window);

	if (fabs(highdpi_tmp - highdpi) > 1e-8)
	{
		post_resize(window,width, height);
		highdpi = highdpi_tmp;
	}

	for (auto& core : core_list)
	{
		core.clear_framebuffers();
	}

	for (auto& core : core_list)
	{
		for (auto& mesh : scn->data_list)
		{
			if (mesh.is_visible & core.id)
			{
				core.draw(scn->MakeTrans(),mesh);
			}
		}
	}

}

void Renderer::SetScene(igl::opengl::glfw::Viewer* viewer)
{
	scn = viewer;
}

IGL_INLINE void Renderer::init(igl::opengl::glfw::Viewer* viewer)
{
	scn = viewer;
	core().init(); 

	core().align_camera_center(scn->data().V, scn->data().F);
}

void Renderer::UpdatePosition(double xpos, double ypos)
{
	xrel = xold - xpos;
	yrel = yold - ypos;
	xold = xpos;
	yold = ypos;
}

void Renderer::MouseProcessing(int button)
{
	
	if (button == 1)
	{

		scn->data().MyTranslate(Eigen::Vector3f(-xrel / 2000.0f, 0, 0));
		scn->data().MyTranslate(Eigen::Vector3f(0,yrel / 2000.0f,0));
		
	}
	else
	{
		scn->data().MyRotate(Eigen::Vector3f(1,0,0),xrel / 180.0f);
		scn->data().MyRotate(Eigen::Vector3f(0, 0,1),yrel / 180.0f);
	}
	
}

Renderer::~Renderer()
{
	//if (scn)
	//	delete scn;
}

bool Renderer::Picking(double newx, double newy)
{
		int fid;
		//Eigen::MatrixXd C = Eigen::MatrixXd::Constant(scn->data().F.rows(), 3, 1);
		Eigen::Vector3f bc;
		double x = newx;
		double y = core().viewport(3) - newy;
		Eigen::Matrix4f view = Eigen::Matrix4f::Identity();
		igl::look_at(core().camera_eye, core().camera_center, core().camera_up, view);
		view = view * (core().trackball_angle * Eigen::Scaling(core().camera_zoom * core().camera_base_zoom)
				* Eigen::Translation3f(core().camera_translation + core().camera_base_translation)).matrix() * scn->MakeTrans() * scn->data().MakeTrans();
		if (igl::unproject_onto_mesh(Eigen::Vector2f(x, y), view,
 			core().proj, core().viewport, scn->data().V, scn->data().F, fid, bc))
		{
			return true;
		}
		return false;
	
}

IGL_INLINE void Renderer::resize(GLFWwindow* window,int w, int h)
	{
		if (window) {
			glfwSetWindowSize(window, w / highdpi, h / highdpi);
		}
		post_resize(window,w, h);
	}

	IGL_INLINE void Renderer::post_resize(GLFWwindow* window, int w, int h)
	{
		if (core_list.size() == 1)
		{
			core().viewport = Eigen::Vector4f(0, 0, w, h);
		}
		else
		{
			// It is up to the user to define the behavior of the post_resize() function
			// when there are multiple viewports (through the `callback_post_resize` callback)
		}
		//for (unsigned int i = 0; i < plugins.size(); ++i)
		//{
		//	plugins[i]->post_resize(w, h);
		//}
		if (callback_post_resize)
		{
			callback_post_resize(window, w, h);
		}
	}

	IGL_INLINE igl::opengl::ViewerCore& Renderer::core(unsigned core_id /*= 0*/)
	{
		assert(!core_list.empty() && "core_list should never be empty");
		int core_index;
		if (core_id == 0)
			core_index = selected_core_index;
		else
			core_index = this->core_index(core_id);
		assert((core_index >= 0 && core_index < core_list.size()) && "selected_core_index should be in bounds");
		return core_list[core_index];
	}

	IGL_INLINE const igl::opengl::ViewerCore& Renderer::core(unsigned core_id /*= 0*/) const
	{
		assert(!core_list.empty() && "core_list should never be empty");
		int core_index;
		if (core_id == 0)
			core_index = selected_core_index;
		else
			core_index = this->core_index(core_id);
		assert((core_index >= 0 && core_index < core_list.size()) && "selected_core_index should be in bounds");
		return core_list[core_index];
	}

	IGL_INLINE bool Renderer::erase_core(const size_t index)
	{
		assert((index >= 0 && index < core_list.size()) && "index should be in bounds");
		//assert(data_list.size() >= 1);
		if (core_list.size() == 1)
		{
			// Cannot remove last viewport
			return false;
		}
		core_list[index].shut(); // does nothing
		core_list.erase(core_list.begin() + index);
		if (selected_core_index >= index && selected_core_index > 0)
		{
			selected_core_index--;
		}
		return true;
	}

	IGL_INLINE size_t Renderer::core_index(const int id) const {
		for (size_t i = 0; i < core_list.size(); ++i)
		{
			if (core_list[i].id == id)
				return i;
		}
		return 0;
	}

	IGL_INLINE int Renderer::append_core(Eigen::Vector4f viewport, bool append_empty /*= false*/)
	{
		core_list.push_back(core()); // copies the previous active core and only changes the viewport
		core_list.back().viewport = viewport;
		core_list.back().id = next_core_id;
		next_core_id <<= 1;
		if (!append_empty)
		{
			for (auto& data : scn->data_list)
			{
				data.set_visible(true, core_list.back().id);
				//data.copy_options(core(), core_list.back());
			}
		}
		selected_core_index = core_list.size() - 1;
		return core_list.back().id;
	}

	//IGL_INLINE void Viewer::select_hovered_core()
	//{
	//	int width_window, height_window = 800;
	//   glfwGetFramebufferSize(window, &width_window, &height_window);
	//	for (int i = 0; i < core_list.size(); i++)
	//	{
	//		Eigen::Vector4f viewport = core_list[i].viewport;

	//		if ((current_mouse_x > viewport[0]) &&
	//			(current_mouse_x < viewport[0] + viewport[2]) &&
	//			((height_window - current_mouse_y) > viewport[1]) &&
	//			((height_window - current_mouse_y) < viewport[1] + viewport[3]))
	//		{
	//			selected_core_index = i;
	//			break;
	//		}
	//	}
	//}