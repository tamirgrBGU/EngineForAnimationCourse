#pragma once
#include "igl/opengl/glfw/Display.h"
//#include "igl/opengl/igl::opengl::MeshGL.h"

static void glfw_mouse_press(GLFWwindow* window, int button, int action, int modifier)
{
  igl::opengl::glfw::Viewer::MouseButton mb;
  Renderer* rndr = (Renderer*) glfwGetWindowUserPointer(window);
  if (button == GLFW_MOUSE_BUTTON_1)
    mb = igl::opengl::glfw::Viewer::MouseButton::Left;
  else if (button == GLFW_MOUSE_BUTTON_2)
    mb = igl::opengl::glfw::Viewer::MouseButton::Right;
  else //if (button == GLFW_MOUSE_BUTTON_3)
    mb = igl::opengl::glfw::Viewer::MouseButton::Middle;

  //if (action == GLFW_PRESS)
  //  __viewer->mouse_down(mb,modifier);
  //else
  //  __viewer->mouse_up(mb,modifier);
}


//static void glfw_char_mods_callback(GLFWwindow* window, unsigned int codepoint, int modifier)
//{
//  __viewer->key_pressed(codepoint, modifier);
//}

 void glfw_window_size(GLFWwindow* window, int width, int height)
{
	Renderer* rndr = (Renderer*) glfwGetWindowUserPointer(window);
	igl::opengl::glfw::Viewer* scn = rndr->GetScene();
	int w = width;// *highdpi;
	int h = height;// *highdpi;

  //renderer->post_resize(w, h);

}

 void glfw_mouse_move(GLFWwindow* window, double x, double y)
{
	 Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
  //scn->mouse_move(x, y);
}

static void glfw_mouse_scroll(GLFWwindow* window, double x, double y)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	
  //using namespace std;
  //scroll_x += x;
 // scroll_y += y;

 // __viewer->mouse_scroll(y);
}

//static void glfw_drop_callback(GLFWwindow *window,int count,const char **filenames)
//{
//
//}

//static void glfw_error_callback(int error, const char* description)
//{
//	fputs(description, stderr);
//}

static void glfw_key_callback(GLFWwindow* window, int key, int scancode, int action, int modifier)
{
	Renderer* rndr = (Renderer*) glfwGetWindowUserPointer(window);
	igl::opengl::glfw::Viewer* scn = rndr->GetScene();
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	//if (action == GLFW_PRESS)
	//  __viewer->key_down(key, modifier);
	//else if(action == GLFW_RELEASE)
	//  __viewer->key_up(key, modifier);
	else if(action == GLFW_PRESS)
		switch (key)
		{
		//case 'A':
		//case 'a':
		//{
		//	rndr->core().is_animating = !rndr->core().is_animating;
		//	break;
		//}
		case 'F':
		case 'f':
		{
			scn->data().set_face_based(!scn->data().face_based);
			break;
		}
		case 'I':
		case 'i':
		{
			scn->data().dirty |= igl::opengl::MeshGL::DIRTY_NORMAL;
			scn->data().invert_normals = !scn->data().invert_normals;
			break;
		}
		case 'L':
		case 'l':
		{
			rndr->core().toggle(scn->data().show_lines);
			break;
		}
		case 'O':
		case 'o':
		{
			rndr->core().orthographic = !rndr->core().orthographic;
			break;
		}
		case 'T':
		case 't':
		{
			rndr->core().toggle(scn->data().show_faces);
			break;
		}
		//case 'Z':
		//{
		//	rndr->snap_to_canonical_quaternion();
		//	break;
		//}
		//case '[':
		//case ']':
		//{
		//	if (rndr->core().rotation_type == ViewerCore::ROTATION_TYPE_TRACKBALL)
		//		rndr->core().set_rotation_type(ViewerCore::ROTATION_TYPE_TWO_AXIS_VALUATOR_FIXED_UP);
		//	else
		//		rndr->core().set_rotation_type(ViewerCore::ROTATION_TYPE_TRACKBALL);

		//	break;
		//}
		case '<':
		case '>':
		{
			scn->selected_data_index =
				(scn->selected_data_index + scn->data_list.size() + (key == '>' ? 1 : -1)) % scn->data_list.size();
			break;
		}
		//case '{':
		//case '}':
		//{
		//	selected_core_index =
		//		(selected_core_index + core_list.size() + (unicode_key == '}' ? 1 : -1)) % core_list.size();
		//	break;
		//}
		case ';':
			scn->data().show_vertid = !scn->data().show_vertid;
			break;
		case ':':
			scn->data().show_faceid = !scn->data().show_faceid;
			break;
		default: break;//do nothing
		}
}


void Init(Display& display)
{
	display.AddKeyCallBack(glfw_key_callback);
	display.AddMouseCallBacks(glfw_mouse_press, glfw_mouse_scroll, glfw_mouse_move);
	display.AddResizeCallBack(glfw_window_size);
}
