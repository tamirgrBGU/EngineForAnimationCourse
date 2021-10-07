#pragma once
#include <igl/igl_inline.h>
#include <vector>
#include <functional>
#include <igl/opengl/ViewerCore.h>
#include <igl/opengl/glfw/Viewer.h>
#include <igl/opengl/glfw/imgui/ImGuiMenu.h>
#include <igl/opengl/glfw/imgui/ImGuiHelpers.h>
#include <../imgui/imgui.h>
struct GLFWwindow;


class Renderer 
{
public:
	Renderer();
	~Renderer();
	IGL_INLINE void draw( GLFWwindow* window);
	IGL_INLINE void init(igl::opengl::glfw::Viewer* scn,int coresNum, igl::opengl::glfw::imgui::ImGuiMenu *_menu);
	
	//IGL_INLINE bool key_pressed(unsigned int unicode_key, int modifiers);

		// Returns **true** if action should be cancelled.
	std::function<bool(GLFWwindow* window)> callback_init;
	std::function<bool(GLFWwindow* window)> callback_pre_draw;
	std::function<bool(GLFWwindow* window)> callback_post_draw;
	std::function<bool(GLFWwindow* window, int button, int modifier)> callback_mouse_down;
	std::function<bool(GLFWwindow* window, int button, int modifier)> callback_mouse_up;
	std::function<bool(GLFWwindow* window, int mouse_x, int mouse_y)> callback_mouse_move;
	std::function<bool(GLFWwindow* window, float delta_y)> callback_mouse_scroll;
	std::function<bool(GLFWwindow* window, unsigned int key, int modifiers)> callback_key_pressed;
	std::function<bool(GLFWwindow* window, int w, int h)> callback_post_resize;
	// THESE SHOULD BE DEPRECATED:
	std::function<bool(GLFWwindow* window, unsigned int key, int modifiers)> callback_key_down;
	std::function<bool(GLFWwindow* window, unsigned int key, int modifiers)> callback_key_up;
	// Pointers to per-callback data
	void* callback_init_data;
	void* callback_pre_draw_data;
	void* callback_post_draw_data;
	void* callback_mouse_down_data;
	void* callback_mouse_up_data;
	void* callback_mouse_move_data;
	void* callback_mouse_scroll_data;
	void* callback_key_pressed_data;
	void* callback_key_down_data;
	void* callback_key_up_data;


	////////////////////////////
	// Multi-viewport methods //
	////////////////////////////

	// Return the current viewport, or the viewport corresponding to a given unique identifier
	//
	// Inputs:
	//   core_id  unique identifier corresponding to the desired viewport (current viewport if 0)
	IGL_INLINE igl::opengl::ViewerCore& core(unsigned core_id = 0);
	IGL_INLINE const igl::opengl::ViewerCore& core(unsigned core_id = 0) const;

	// Append a new "slot" for a viewport (i.e., copy properties of the current viewport, only
	// changing the viewport size/position)
	//
	// Inputs:
	//   viewport      Vector specifying the viewport origin and size in screen coordinates.
	//   append_empty  If true, existing meshes are hidden on the new viewport.
	//
	// Returns the unique id of the newly inserted viewport. There can be a maximum of 31
	//   viewports created in the same viewport. Erasing a viewport does not change the id of
	//   other existing viewports
	IGL_INLINE int append_core(Eigen::Vector4f viewport, bool append_empty = false);

	// Erase a viewport
	//
	// Inputs:
	//   index  index of the viewport to erase
	IGL_INLINE bool erase_core(const size_t index);

	// Retrieve viewport index from its unique identifier
	// Returns 0 if not found
	IGL_INLINE size_t core_index(const int id) const;

	// Change selected_core_index to the viewport containing the mouse
	// (current_mouse_x, current_mouse_y)
	// IGL_INLINE void select_hovered_core();

	// Callbacks
	 double Picking(double x, double y);
	 inline void Animate() { scn->Animate(); };
	IGL_INLINE bool key_pressed(unsigned int unicode_key, int modifier);
	IGL_INLINE void resize(GLFWwindow* window,int w, int h); // explicitly set window size
	IGL_INLINE void post_resize(GLFWwindow* window, int w, int h); // external resize due to user interaction
	void SetScene(igl::opengl::glfw::Viewer* scn);
	void UpdatePosition(double xpos, double ypos);
	void MouseProcessing(int button);
	inline igl::opengl::glfw::Viewer* GetScene() {
		return scn;
	}
	inline void ChangeCamera(int unicode_key)
	{
		selected_core_index =
			(selected_core_index + core_list.size() + (unicode_key == ']' ? 1 : -1)) % core_list.size();

	}
	void TranslateCamera(Eigen::Vector3f amt);
	void RotateCamera(float amtX, float amtY);
	inline bool IsPicked() { return scn->isPicked; }
	
private:
	// Stores all the viewing options
	std::vector<igl::opengl::ViewerCore> core_list;
	igl::opengl::glfw::Viewer* scn;
	size_t selected_core_index;
	int next_core_id;
	float highdpi;
	double xold, yold, xrel, yrel;
	float depth;
	unsigned int left_view, right_view;
	double doubleVariable;
	igl::opengl::glfw::imgui::ImGuiMenu* menu;
	double z;
};

