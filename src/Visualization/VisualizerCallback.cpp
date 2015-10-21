// ----------------------------------------------------------------------------
// -                       Open3DV: www.open3dv.org                           -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2015 Qianyi Zhou <Qianyi.Zhou@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "Visualizer.h"

namespace three{

void Visualizer::WindowRefreshCallback(GLFWwindow *window)
{
	if (is_redraw_required_) {
		Render();
		is_redraw_required_ = false;
	}
}

void Visualizer::WindowResizeCallback(GLFWwindow *window, int w, int h)
{
	view_control_ptr_->ChangeWindowSize(w, h);
	is_redraw_required_ = true;
	for (auto geometry_ptr : geometry_ptrs_) {
		if (geometry_ptr->GetGeometryType() == Geometry::GEOMETRY_IMAGE) {
			UpdateGeometry();
		}
	}
}

void Visualizer::MouseMoveCallback(GLFWwindow *window, double x, double y)
{
	if (mouse_control_.is_mouse_left_button_down) {
		if (mouse_control_.is_control_key_down) {
			view_control_ptr_->Translate(
					mouse_control_.mouse_position_x - x,
					y - mouse_control_.mouse_position_y);
		} else {
			view_control_ptr_->Rotate(
					mouse_control_.mouse_position_x - x,
					y - mouse_control_.mouse_position_y);
		}
	}
	mouse_control_.mouse_position_x = x;
	mouse_control_.mouse_position_y = y;
	is_redraw_required_ = true;
}

void Visualizer::MouseScrollCallback(GLFWwindow* window, double x, double y)
{
	view_control_ptr_->Scale(y);
	is_redraw_required_ = true;
}

void Visualizer::MouseButtonCallback(GLFWwindow* window,
		int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT) {
		if (action == GLFW_PRESS) {
			mouse_control_.is_mouse_left_button_down = true;
			if (mods & GLFW_MOD_CONTROL) {
				mouse_control_.is_control_key_down = true;
			} else {
				mouse_control_.is_control_key_down = false;
			}
		} else {
			mouse_control_.is_mouse_left_button_down = false;
			mouse_control_.is_control_key_down = false;
		}
	}
}

void Visualizer::KeyPressCallback(GLFWwindow *window,
		int key, int scancode, int action, int mods)
{
	if (action == GLFW_RELEASE) {
		return;
	}

	switch (key) {
	case GLFW_KEY_ESCAPE:
	case GLFW_KEY_Q:
		glfwSetWindowShouldClose(window_, GL_TRUE);
		PrintDebug("[Visualizer] Window closing.\n");
		break;
	case GLFW_KEY_H:
		PrintVisualizerHelp();
		break;
	case GLFW_KEY_R:
		ResetViewPoint();
		PrintDebug("[Visualizer] Reset view point.\n");
		break;
	case GLFW_KEY_P:
	case GLFW_KEY_PRINT_SCREEN:
		CaptureScreen();
		break;
	case GLFW_KEY_LEFT_BRACKET:
		view_control_ptr_->ChangeFieldOfView(-1.0);
		PrintDebug("[Visualizer] Field of view set to %.2f.\n",
				view_control_ptr_->GetFieldOfView());
		break;
	case GLFW_KEY_RIGHT_BRACKET:
		view_control_ptr_->ChangeFieldOfView(1.0);
		PrintDebug("[Visualizer] Field of view set to %.2f.\n",
				view_control_ptr_->GetFieldOfView());
		break;
	case GLFW_KEY_EQUAL:
		pointcloud_render_mode_.ChangePointSize(1.0);
		if (pointcloud_render_mode_.IsNormalShown()) {
			UpdateGeometry();
		}
		PrintDebug("[Visualizer] Point size set to %.2f.\n",
				pointcloud_render_mode_.GetPointSize());
		break;
	case GLFW_KEY_MINUS:
		pointcloud_render_mode_.ChangePointSize(-1.0);
		if (pointcloud_render_mode_.IsNormalShown()) {
			UpdateGeometry();
		}
		PrintDebug("[Visualizer] Point size set to %.2f.\n",
				pointcloud_render_mode_.GetPointSize());
		break;
	case GLFW_KEY_N:
		pointcloud_render_mode_.ToggleShowNormal();
		UpdateGeometry();
		PrintDebug("[Visualizer] Point normal rendering %s.\n",
				pointcloud_render_mode_.IsNormalShown() ? "ON" : "OFF");
		break;
	case GLFW_KEY_S:
		mesh_render_mode_.ToggleShadingOption();
		UpdateGeometry();
		PrintDebug("[Visualizer] Mesh shading mode is %s.\n",
				mesh_render_mode_.GetMeshShadeOption() == 
				TriangleMeshRenderMode::MESHSHADE_FLATSHADE ? 
				"FLAT" : "SMOOTH");
		break;
	case GLFW_KEY_B:
		mesh_render_mode_.ToggleShowBackFace();
		PrintDebug("[Visualizer] Mesh back face rendering %s.\n",
				mesh_render_mode_.IsBackFaceShown() ? "ON" : "OFF");
		break;
	case GLFW_KEY_I:
		image_render_mode_.ToggleInterpolationOption();
		UpdateGeometry();
		PrintDebug("[Visualizer] Image interpolation mode is %s.\n",
				image_render_mode_.GetInterpolationOption() == 
				ImageRenderMode::IMAGE_INTERPOLATION_NEAREST ? 
				"NEARST" : "LINEAR");
		break;
	case GLFW_KEY_T:
		image_render_mode_.ToggleImageStretchOption();
		UpdateGeometry();
		PrintDebug("[Visualizer] Image stretch mode is #%d.\n",
				int(image_render_mode_.GetImageStretchOption()));
		break;
	case GLFW_KEY_0:
		if (mods & GLFW_MOD_CONTROL) {
			mesh_render_mode_.SetMeshColorOption(
					TriangleMeshRenderMode::TRIANGLEMESH_DEFAULT);
			UpdateGeometry();
			PrintDebug("[Visualizer] Mesh color set to DEFAULT.\n");
		} else if (mods & GLFW_MOD_SHIFT) {
			SetGlobalColorMap(ColorMap::COLORMAP_GRAY);
			UpdateGeometry();
			PrintDebug("[Visualizer] Color map set to GRAY.\n");
		} else {
			pointcloud_render_mode_.SetPointColorOption(
					PointCloudRenderMode::POINTCOLOR_DEFAULT);
			UpdateGeometry();
			PrintDebug("[Visualizer] Point color set to DEFAULT.\n");
		}
		break;
	case GLFW_KEY_1:
		if (mods & GLFW_MOD_CONTROL) {
			mesh_render_mode_.SetMeshColorOption(
					TriangleMeshRenderMode::TRIANGLEMESH_COLOR);
			UpdateGeometry();
			PrintDebug("[Visualizer] Mesh color set to COLOR.\n");
		} else if (mods & GLFW_MOD_SHIFT) {
			SetGlobalColorMap(ColorMap::COLORMAP_JET);
			UpdateGeometry();
			PrintDebug("[Visualizer] Color map set to JET.\n");
		} else {
			pointcloud_render_mode_.SetPointColorOption(
					PointCloudRenderMode::POINTCOLOR_COLOR);
			UpdateGeometry();
			PrintDebug("[Visualizer] Point color set to COLOR.\n");
		}
		break;
	case GLFW_KEY_2:
		if (mods & GLFW_MOD_CONTROL) {
			mesh_render_mode_.SetMeshColorOption(
					TriangleMeshRenderMode::TRIANGLEMESH_X);
			UpdateGeometry();
			PrintDebug("[Visualizer] Mesh color set to X.\n");
		} else if (mods & GLFW_MOD_SHIFT) {
			SetGlobalColorMap(ColorMap::COLORMAP_SUMMER);
			UpdateGeometry();
			PrintDebug("[Visualizer] Color map set to SUMMER.\n");
		} else {
			pointcloud_render_mode_.SetPointColorOption(
					PointCloudRenderMode::POINTCOLOR_X);
			UpdateGeometry();
			PrintDebug("[Visualizer] Point color set to X.\n");
		}
		break;
	case GLFW_KEY_3:
		if (mods & GLFW_MOD_CONTROL) {
			mesh_render_mode_.SetMeshColorOption(
					TriangleMeshRenderMode::TRIANGLEMESH_Y);
			UpdateGeometry();
			PrintDebug("[Visualizer] Mesh color set to Y.\n");
		} else if (mods & GLFW_MOD_SHIFT) {
			SetGlobalColorMap(ColorMap::COLORMAP_WINTER);
			UpdateGeometry();
			PrintDebug("[Visualizer] Color map set to WINTER.\n");
		} else {
			pointcloud_render_mode_.SetPointColorOption(
					PointCloudRenderMode::POINTCOLOR_Y);
			UpdateGeometry();
			PrintDebug("[Visualizer] Point color set to Y.\n");
		}
		break;
	case GLFW_KEY_4:
		if (mods & GLFW_MOD_CONTROL) {
			mesh_render_mode_.SetMeshColorOption(
					TriangleMeshRenderMode::TRIANGLEMESH_Z);
			UpdateGeometry();
			PrintDebug("[Visualizer] Mesh color set to Z.\n");
		} else if (mods & GLFW_MOD_SHIFT) {
		} else {
			pointcloud_render_mode_.SetPointColorOption(
					PointCloudRenderMode::POINTCOLOR_Z);
			UpdateGeometry();
			PrintDebug("[Visualizer] Point color set to Z.\n");
		}
		break;
	default:
		break;
	}

	is_redraw_required_ = true;
}

void Visualizer::WindowCloseCallback(GLFWwindow *window)
{
	// happens when user click the close icon to close the window
}

}	// namespace three