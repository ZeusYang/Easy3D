/*
*	Copyright (C) 2015 by Liangliang Nan (liangliang.nan@gmail.com)
*
*	This file is part of EasyGUI: software for processing and rendering
*   meshes and point clouds.
*
*	EasyGUI is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License Version 3
*	as published by the Free Software Foundation.
*
*	EasyGUI is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#include "window.h"
#include "plugin.h"

#include <iostream>

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <GLFW/glfw3.h>

#include "viewer.h"
#include "imgui_fonts_droid_sans.h"
#include "ImGuiHelpers.h"


ImGuiContext* Window::context_ = nullptr;


Window::Window(Viewer* viewer, const std::string& title) 
	: name_(title)
{
	viewer_ = viewer;
	viewer_->windows_.push_back(this);
}


void Window::init() {
	if (!context_) {
		// Setup ImGui binding
		IMGUI_CHECKVERSION();

		context_ = ImGui::CreateContext();

		const char* glsl_version = "#version 150";
		ImGui_ImplGlfw_InitForOpenGL(viewer_->window_, false);
		ImGui_ImplOpenGL3_Init(glsl_version);
		ImGuiIO& io = ImGui::GetIO();
		io.IniFilename = nullptr;
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.FrameRounding = 5.0f;

		// load font
		reload_font();
	}
}

void Window::reload_font(int font_size)
{
	ImGuiIO& io = ImGui::GetIO();
    io.Fonts->Clear();
	//io.Fonts->AddFontFromFileTTF("../3rd_party/imgui/misc/fonts/Roboto-Medium.ttf", 26.0f);
	io.Fonts->AddFontFromMemoryCompressedTTF(droid_sans_compressed_data, droid_sans_compressed_size, font_size * hidpi_scaling());
    io.FontGlobalScale = 1.0f / pixel_ratio();
}


void Window::cleanup()
{
	for (auto p : plugins_)
		p->cleanup();
}


void Window::post_resize(int width, int height)
{
	if (context_)
	{
		ImGui::GetIO().DisplaySize.x = float(width);
		ImGui::GetIO().DisplaySize.y = float(height);
	}
}

// Mouse IO
bool Window::mouse_press(int button, int modifier)
{
	ImGui_ImplGlfw_MouseButtonCallback(viewer_->window_, button, GLFW_PRESS, modifier);
	return ImGui::GetIO().WantCaptureMouse;
}

bool Window::mouse_release(int button, int modifier)
{
	return ImGui::GetIO().WantCaptureMouse;
}

bool Window::mouse_move(int mouse_x, int mouse_y)
{
	return ImGui::GetIO().WantCaptureMouse;
}

bool Window::mouse_scroll(double delta_y)
{
	ImGui_ImplGlfw_ScrollCallback(viewer_->window_, 0.f, delta_y);
	return ImGui::GetIO().WantCaptureMouse;
}

// Keyboard IO
bool Window::char_input(unsigned int key)
{
	ImGui_ImplGlfw_CharCallback(nullptr, key);
	return ImGui::GetIO().WantCaptureKeyboard;
}

bool Window::key_press(int key, int modifiers)
{
	ImGui_ImplGlfw_KeyCallback(viewer_->window_, key, 0, GLFW_PRESS, modifiers);
	return ImGui::GetIO().WantCaptureKeyboard;
}

bool Window::key_release(int key, int modifiers)
{
	ImGui_ImplGlfw_KeyCallback(viewer_->window_, key, 0, GLFW_RELEASE, modifiers);
	return ImGui::GetIO().WantCaptureKeyboard;
}


bool Window::draw()
{
#if 1  // not overlap windows
	std::size_t win_id = 0;
	const std::vector<Window*>& windows = viewer_->windows();
	for (std::size_t i=0; i<windows.size(); ++i) {
		if (windows[i] == this) {
			win_id = i;
		}
	}
	float menu_width = 180.f * menu_scaling();
	float pos_x = 10.0f + (10.0f + menu_width) * win_id;
    ImGui::SetNextWindowPos(ImVec2(pos_x, 10.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize(ImVec2(0.0f, 0.0f), ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSizeConstraints(ImVec2(menu_width, -1.0f), ImVec2(menu_width, -1.0f));
#endif 

	static bool _viewer_menu_visible = true;
	ImGui::Begin(
        name_.c_str(), &_viewer_menu_visible,
        ImGuiWindowFlags_NoSavedSettings
        | ImGuiWindowFlags_AlwaysAutoResize

//                ImGuiWindowFlags_NoTitleBar
//                //| ImGuiWindowFlags_NoResize
//                | ImGuiWindowFlags_AlwaysAutoResize
//                | ImGuiWindowFlags_NoMove
//                | ImGuiWindowFlags_NoScrollbar
//                | ImGuiWindowFlags_NoScrollWithMouse
//                | ImGuiWindowFlags_NoCollapse
//                | ImGuiWindowFlags_NoSavedSettings
//                    //| ImGuiWindowFlags_NoInputs
	);
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.4f);

	draw_widgets();
	for (auto p : plugins_)
		p->draw();

    ImGui::PopItemWidth();
	ImGui::End();
	return false;
}


void Window::draw_widgets()
{
	// Workspace
	if (ImGui::CollapsingHeader("Workspace", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float w = ImGui::GetContentRegionAvailWidth();
		float p = ImGui::GetStyle().FramePadding.x;
		if (ImGui::Button("Load##Workspace", ImVec2((w - p) / 2.f, 0)))
		{
			//viewer_->load_scene();
		}
		ImGui::SameLine(0, p);
		if (ImGui::Button("Save##Workspace", ImVec2((w - p) / 2.f, 0)))
		{
			//viewer_->save_scene();
		}
	}

	// Mesh
	if (ImGui::CollapsingHeader("Mesh", ImGuiTreeNodeFlags_DefaultOpen))
	{
		float w = ImGui::GetContentRegionAvailWidth();
		float p = ImGui::GetStyle().FramePadding.x;
		if (ImGui::Button("Load##Mesh", ImVec2((w - p) / 2.f, 0)))
		{
			//viewer_->open_dialog_load_mesh();
		}
		ImGui::SameLine(0, p);
		if (ImGui::Button("Save##Mesh", ImVec2((w - p) / 2.f, 0)))
		{
			//viewer_->open_dialog_save_mesh();
		}
	}
}



float Window::pixel_ratio()
{
	// Computes pixel ratio for hidpi devices
	int buf_size[2];
	int win_size[2];
	GLFWwindow* window = glfwGetCurrentContext();
	glfwGetFramebufferSize(window, &buf_size[0], &buf_size[1]);
	glfwGetWindowSize(window, &win_size[0], &win_size[1]);
	return (float)buf_size[0] / (float)win_size[0];
}

float Window::hidpi_scaling()
{
	// Computes scaling factor for hidpi devices
	float xscale, yscale;
	GLFWwindow* window = glfwGetCurrentContext();
	glfwGetWindowContentScale(window, &xscale, &yscale);
	return 0.5f * (xscale + yscale);
}