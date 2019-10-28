

#include <chrono>
#include <thread>

#include "../gl.h"
#include "Display.h"

#include "igl/igl_inline.h"
#include <igl/get_seconds.h>


static void glfw_error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

Display::Display(int windowWidth, int windowHeight, const std::string& title)
{
	bool resizable = true, fullscreen = false;
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
		{
			exit(EXIT_FAILURE);
		}
		glfwWindowHint(GLFW_SAMPLES, 8);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		
//#ifdef __APPLE__
//		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
//		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif
//		if (fullscreen)
//		{
//			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
//			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
//			window = glfwCreateWindow(mode->width, mode->height, title.c_str(), monitor, nullptr);
//			windowWidth = mode->width;
//			windowHeight = mode->height;
//		}
//		else
//		{
			// Set default windows width
			//if (windowWidth <= 0 & core_list.size() == 1 && renderer->core().viewport[2] > 0)
			//	windowWidth = renderer->core().viewport[2];
			//else 
			//	if (windowWidth <= 0)
			//	windowWidth = 1280;
			//// Set default windows height
			//if (windowHeight <= 0 & core_list.size() == 1 && renderer->core().viewport[3] > 0)
			//	windowHeight = renderer->core().viewport[3];
			//else if (windowHeight <= 0)
			//	windowHeight = 800;
//			window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);
//		}
		window = glfwCreateWindow(windowWidth, windowHeight, title.c_str(), nullptr, nullptr);
		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glfwMakeContextCurrent(window);
		// Load OpenGL and its extensions
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			printf("Failed to load OpenGL and its extensions\n");
			exit(EXIT_FAILURE);
		}
#if defined(DEBUG) || defined(_DEBUG)
		printf("OpenGL Version %d.%d loaded\n", GLVersion.major, GLVersion.minor);
		int major, minor, rev;
		major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
		minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
		rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
		printf("OpenGL version received: %d.%d.%d\n", major, minor, rev);
		printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
		printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		//Tamir: changes from here
		// Initialize FormScreen
	   // __viewer = this;
		// Register callbacks
		//glfwSetKeyCallback(window, glfw_key_callback);
		//glfwSetCursorPosCallback(window,glfw_mouse_move);
		//glfwSetScrollCallback(window, glfw_mouse_scroll);
		//glfwSetMouseButtonCallback(window, glfw_mouse_press);
		//glfwSetWindowSizeCallback(window,glfw_window_size);
	
		
		//glfwSetCharModsCallback(window,glfw_char_mods_callback);
		//glfwSetDropCallback(window,glfw_drop_callback);
		// Handle retina displays (windows and mac)
		//int width, height;
		//glfwGetFramebufferSize(window, &width, &height);
		//int width_window, height_window;
		//glfwGetWindowSize(window, &width_window, &height_window);
		//highdpi = windowWidth/width_window;
		
		//glfw_window_size(window,width_window,height_window);
		//opengl.init();
//		core().align_camera_center(data().V, data().F);
		// Initialize IGL viewer
//		init();
		
}

bool Display::launch_rendering(bool loop)
{
	// glfwMakeContextCurrent(window);
	// Rendering loop
	const int num_extra_frames = 5;
	int frame_counter = 0;
	int windowWidth, windowHeight;
	//main loop
	Renderer* renderer = (Renderer*)glfwGetWindowUserPointer(window);
	glfwGetWindowSize(window, &windowWidth, &windowHeight);
	renderer->post_resize(window, windowWidth, windowHeight);
	
	while (!glfwWindowShouldClose(window))
	{

		double tic = igl::get_seconds();
		renderer->draw(window);
		glfwSwapBuffers(window);
		if (renderer->core().is_animating || frame_counter++ < num_extra_frames)
		{//motion
			glfwPollEvents();
			// In microseconds
			double duration = 1000000. * (igl::get_seconds() - tic);
			const double min_duration = 1000000. / renderer->core().animation_max_fps;
			if (duration < min_duration)
			{
				std::this_thread::sleep_for(std::chrono::microseconds((int)(min_duration - duration)));
			}
		}
		else
		{
			glfwWaitEvents();
			frame_counter = 0;
		}
		if (!loop)
			return !glfwWindowShouldClose(window);

#ifdef __APPLE__
		static bool first_time_hack = true;
		if (first_time_hack) {
			glfwHideWindow(window);
			glfwShowWindow(window);
			first_time_hack = false;
		}
#endif
	}
	return EXIT_SUCCESS;
}

void Display::AddKeyCallBack(void(*keyCallback)(GLFWwindow*, int, int, int, int))
{
	glfwSetKeyCallback(window, (void(*)(GLFWwindow*, int, int, int, int))keyCallback);//{

}

void Display::AddMouseCallBacks(void (*mousebuttonfun)(GLFWwindow*, int, int, int), void (*scrollfun)(GLFWwindow*, double, double), void (*cursorposfun)(GLFWwindow*, double, double))
{
	glfwSetMouseButtonCallback(window, mousebuttonfun);
	glfwSetScrollCallback(window, scrollfun);
	glfwSetCursorPosCallback(window, cursorposfun);
}

void Display::AddResizeCallBack(void (*windowsizefun)(GLFWwindow*, int, int))
{
	glfwSetWindowSizeCallback(window, windowsizefun);
}

void Display::SetRenderer(void* userPointer)
{
	
	glfwSetWindowUserPointer(window, userPointer);
	
}

void* Display::GetScene()
{
	return glfwGetWindowUserPointer(window);
}

void Display::SwapBuffers()
{
	glfwSwapBuffers(window);
}

void Display::PollEvents()
{
	glfwPollEvents();
}

Display::~Display()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

//IGL_INLINE bool Display::key_pressed(unsigned int unicode_key, int modifiers)
//{
//	//for (unsigned int i = 0; i < plugins.size(); ++i)
//	//{
//	//	if (plugins[i]->key_pressed(unicode_key, modifiers))
//	//	{
//	//		return true;
//	//	}
//	//}
//
//	//if (callback_key_pressed)
//	//	if (callback_key_pressed(*this, unicode_key, modifiers))
//	//		return true;
//
//	switch (unicode_key)
//	{
//	case 'A':
//	case 'a':
//	{
//		core().is_animating = !core().is_animating;
//		return true;
//	}
//	case 'F':
//	case 'f':
//	{
//		data().set_face_based(!data().face_based);
//		return true;
//	}
//	case 'I':
//	case 'i':
//	{
//		data().dirty |= MeshGL::DIRTY_NORMAL;
//		data().invert_normals = !data().invert_normals;
//		return true;
//	}
//	case 'L':
//	case 'l':
//	{
//		core().toggle(data().show_lines);
//		return true;
//	}
//	case 'O':
//	case 'o':
//	{
//		core().orthographic = !core().orthographic;
//		return true;
//	}
//	case 'T':
//	case 't':
//	{
//		core().toggle(data().show_faces);
//		return true;
//	}
//	case 'Z':
//	{
//		snap_to_canonical_quaternion();
//		return true;
//	}
//	case '[':
//	case ']':
//	{
//		if (core().rotation_type == ViewerCore::ROTATION_TYPE_TRACKBALL)
//			core().set_rotation_type(ViewerCore::ROTATION_TYPE_TWO_AXIS_VALUATOR_FIXED_UP);
//		else
//			core().set_rotation_type(ViewerCore::ROTATION_TYPE_TRACKBALL);
//
//		return true;
//	}
//	case '<':
//	case '>':
//	{
//		selected_data_index =
//			(selected_data_index + data_list.size() + (unicode_key == '>' ? 1 : -1)) % data_list.size();
//		return true;
//	}
//	case '{':
//	case '}':
//	{
//		selected_core_index =
//			(selected_core_index + core_list.size() + (unicode_key == '}' ? 1 : -1)) % core_list.size();
//		return true;
//	}
//	case ';':
//		data().show_vertid = !data().show_vertid;
//		return true;
//	case ':':
//		data().show_faceid = !data().show_faceid;
//		return true;
//	default: break;//do nothing
//	}
//	return false;
//	}

	//IGL_INLINE bool Renderer::key_down(int key, int modifiers)
	//{
	//	for (unsigned int i = 0; i < plugins.size(); ++i)
	//		if (plugins[i]->key_down(key, modifiers))
	//			return true;

	//	if (callback_key_down)
	//		if (callback_key_down(*this, key, modifiers))
	//			return true;

	//	return false;
	//}

	//IGL_INLINE bool Renderer::key_up(int key, int modifiers)
	//{
	//	for (unsigned int i = 0; i < plugins.size(); ++i)
	//		if (plugins[i]->key_up(key, modifiers))
	//			return true;

	//	if (callback_key_up)
	//		if (callback_key_up(*this, key, modifiers))
	//			return true;

	//	return false;
	//}

	//IGL_INLINE void Renderer::select_hovered_core()
	//{
	//	int width_window, height_window = 800;
	//	glfwGetFramebufferSize(window, &width_window, &height_window);
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