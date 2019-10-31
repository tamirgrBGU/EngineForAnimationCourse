#pragma once
#include "igl/opengl/glfw/Display.h"

static void glfw_mouse_press(GLFWwindow* window, int button, int action, int modifier)
{

  Renderer* rndr = (Renderer*) glfwGetWindowUserPointer(window);
  
  if (action == GLFW_PRESS)
  {
	  double x2, y2;
	  glfwGetCursorPos(window, &x2, &y2);
	  igl::opengl::glfw::Viewer* scn = rndr->GetScene();
	  bool found = false;
	  int i = 0, savedIndx = scn->selected_data_index;
	  
	  for (; i < scn->data_list.size() && !found;i++)
	  { 
		  scn->selected_data_index = i;
		  found = rndr->Picking(x2, y2);
	  }
	  
	  if(!found)
	  {
		  std::cout << "not found " << std::endl;
		  scn->selected_data_index = savedIndx;
	  }
	  else
		  std::cout << "found " << i - 1 << std::endl;
	  rndr->UpdatePosition(x2, y2);
	 
  }
}


//static void glfw_char_mods_callback(GLFWwindow* window, unsigned int codepoint, int modifier)
//{
//  __viewer->key_pressed(codepoint, modifier);
//}

 void glfw_mouse_move(GLFWwindow* window, double x, double y)
{
	 Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	 rndr->UpdatePosition(x, y);
	 if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
	 {
		 rndr->MouseProcessing(GLFW_MOUSE_BUTTON_RIGHT);
	 }
	 else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	 {
		 rndr->MouseProcessing(GLFW_MOUSE_BUTTON_LEFT);
	 }
}

static void glfw_mouse_scroll(GLFWwindow* window, double x, double y)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	rndr->GetScene()->data().MyScale(Eigen::Vector3f(1 + y * 0.01,1 + y * 0.01,1+y*0.01));

}

void glfw_window_size(GLFWwindow* window, int width, int height)
{
	Renderer* rndr = (Renderer*)glfwGetWindowUserPointer(window);
	//igl::opengl::glfw::Viewer* scn = rndr->GetScene();

    rndr->post_resize(window,width, height);

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

	else if(action == GLFW_PRESS || action == GLFW_REPEAT)
		switch (key)
		{
		case 'A':
		case 'a':
		{
			rndr->core().is_animating = !rndr->core().is_animating;
			break;
		}
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
		case '1':
		case '2':
		{
			scn->selected_data_index =
				(scn->selected_data_index + scn->data_list.size() + (key == '2' ? 1 : -1)) % scn->data_list.size();
			break;
		}
		case '[':
		case ']':
		{
			rndr->ChangeCamera(key);
			break;
		}
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


//IGL_INLINE bool Renderer::mouse_down(igl::opengl::glfw::Viewer::MouseButton button, int modifier)
//{
//	// Remember mouse location at down even if used by callback/plugin
//	down_mouse_x = current_mouse_x;
//	down_mouse_y = current_mouse_y;

//	for (unsigned int i = 0; i < plugins.size(); ++i)
//		if (plugins[i]->mouse_down(static_cast<int>(button), modifier))
//			return true;

//	if (callback_mouse_down)
//		if (callback_mouse_down(*this, static_cast<int>(button), modifier))
//			return true;

//	down = true;

//	// Select the core containing the click location.
//	select_hovered_core();

//	down_translation = core().camera_translation;


//	// Initialization code for the trackball
//	Eigen::RowVector3d center;
//	if (data().V.rows() == 0)
//	{
//		center << 0, 0, 0;
//	}
//	else
//	{
//		center = data().V.colwise().sum() / data().V.rows();
//	}

//	Eigen::Vector3f coord =
//		igl::project(
//			Eigen::Vector3f(center(0), center(1), center(2)),
//			core().view,
//			core().proj,
//			core().viewport);
//	down_mouse_z = coord[2];
//	down_rotation = core().trackball_angle;

//	mouse_mode = MouseMode::Rotation;

//	switch (button)
//	{
//	case MouseButton::Left:
//		if (core().rotation_type == ViewerCore::ROTATION_TYPE_NO_ROTATION) {
//			mouse_mode = MouseMode::Translation;
//		}
//		else {
//			mouse_mode = MouseMode::Rotation;
//		}
//		break;

//	case MouseButton::Right:
//		mouse_mode = MouseMode::Translation;
//		break;

//	default:
//		mouse_mode = MouseMode::None;
//		break;
//	}

//	return true;
//}

//IGL_INLINE bool Renderer::mouse_up(igl::opengl::glfw::Viewer::MouseButton button, int modifier)
//{
//	down = false;

//	for (unsigned int i = 0; i < plugins.size(); ++i)
//		if (plugins[i]->mouse_up(static_cast<int>(button), modifier))
//			return true;

//	if (callback_mouse_up)
//		if (callback_mouse_up(*this, static_cast<int>(button), modifier))
//			return true;

//	mouse_mode = MouseMode::None;

//	return true;
//}

//IGL_INLINE bool Renderer::mouse_move(int mouse_x, int mouse_y)
//{
//	if (hack_never_moved)
//	{
//		down_mouse_x = mouse_x;
//		down_mouse_y = mouse_y;
//		hack_never_moved = false;
//	}
//	current_mouse_x = mouse_x;
//	current_mouse_y = mouse_y;

//	for (unsigned int i = 0; i < plugins.size(); ++i)
//		if (plugins[i]->mouse_move(mouse_x, mouse_y))
//			return true;

//	if (callback_mouse_move)
//		if (callback_mouse_move(*this, mouse_x, mouse_y))
//			return true;


//	if (down)
//	{
//		// We need the window height to transform the mouse click coordinates into viewport-mouse-click coordinates
//		// for igl::trackball and igl::two_axis_valuator_fixed_up
//		int width_window, height_window;
//		glfwGetFramebufferSize(window, &width_window, &height_window);
//		switch (mouse_mode)
//		{
//		case MouseMode::Rotation:
//		{
//			switch (core().rotation_type)
//			{
//			default:
//				assert(false && "Unknown rotation type");
//			case ViewerCore::ROTATION_TYPE_NO_ROTATION:
//				break;
//			case ViewerCore::ROTATION_TYPE_TRACKBALL:
//				igl::trackball(
//					core().viewport(2),
//					core().viewport(3),
//					2.0f,
//					down_rotation,
//					down_mouse_x - core().viewport(0),
//					down_mouse_y - (height_window - core().viewport(1) - core().viewport(3)),
//					mouse_x - core().viewport(0),
//					mouse_y - (height_window - core().viewport(1) - core().viewport(3)),
//					core().trackball_angle);
//				break;
//			case ViewerCore::ROTATION_TYPE_TWO_AXIS_VALUATOR_FIXED_UP:
//				igl::two_axis_valuator_fixed_up(
//					core().viewport(2), core().viewport(3),
//					2.0,
//					down_rotation,
//					down_mouse_x - core().viewport(0),
//					down_mouse_y - (height_window - core().viewport(1) - core().viewport(3)),
//					mouse_x - core().viewport(0),
//					mouse_y - (height_window - core().viewport(1) - core().viewport(3)),
//					core().trackball_angle);
//				break;
//			}
//			//Eigen::Vector4f snapq = core().trackball_angle;

//			break;
//		}

//		case MouseMode::Translation:
//		{
//			//translation
//			Eigen::Vector3f pos1 = igl::unproject(Eigen::Vector3f(mouse_x, core().viewport[3] - mouse_y, down_mouse_z), core().view, core().proj, core().viewport);
//			Eigen::Vector3f pos0 = igl::unproject(Eigen::Vector3f(down_mouse_x, core().viewport[3] - down_mouse_y, down_mouse_z), core().view, core().proj, core().viewport);

//			Eigen::Vector3f diff = pos1 - pos0;
//			core().camera_translation = down_translation + Eigen::Vector3f(diff[0], diff[1], diff[2]);

//			break;
//		}
//		case MouseMode::Zoom:
//		{
//			float delta = 0.001f * (mouse_x - down_mouse_x + mouse_y - down_mouse_y);
//			core().camera_zoom *= 1 + delta;
//			down_mouse_x = mouse_x;
//			down_mouse_y = mouse_y;
//			break;
//		}

//		default:
//			break;
//		}
//	}

//	}

	//IGL_INLINE bool Renderer::mouse_scroll(float delta_y)
	//{
	//	// Direct the scrolling operation to the appropriate viewport
	//	// (unless the core selection is locked by an ongoing mouse interaction).
	//	if (!down)
	//		select_hovered_core();
	//	scroll_position += delta_y;

	//	for (unsigned int i = 0; i < plugins.size(); ++i)
	//		if (plugins[i]->mouse_scroll(delta_y))
	//			return true;

	//	if (callback_mouse_scroll)
	//		if (callback_mouse_scroll(*this, delta_y))
	//			return true;

	//	// Only zoom if there's actually a change
	//	if (delta_y != 0)
	//	{
	//		float mult = (1.0 + ((delta_y > 0) ? 1. : -1.) * 0.05);
	//		const float min_zoom = 0.1f;
	//		core().camera_zoom = (core().camera_zoom * mult > min_zoom ? core().camera_zoom * mult : min_zoom);
	//	}
	//	return true;
	//}