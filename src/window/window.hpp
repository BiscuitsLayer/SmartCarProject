#pragma once

// STL
#include <thread>

// OpenGL Wrapper
#include <GL/Window/Window.hpp>

// ImGui
#include "imgui_impl_win32.h"
#include "imgui_internal.h"

// Sources
#include <config/config_handler.hpp>

// Forward declaration of event handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace App {

// Customized event handler forward declaration
LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// The only purpose of that class is to have customized event handler as a friend function
class CustomWindow: public GL::Window {
	/*
	 *	Customized window class for Win32
	 *	to set up customized event handler for Win32 window
	 *	to be able to handle ImGui events
	 */
public:
	CustomWindow(GL::uint width, GL::uint height, const std::string& title, GL::WindowStyle::window_style_t style);
	CustomWindow(Config::WindowConfig config);

private:
	// Event handler now has access to window's private members
	friend LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

/*
 *	Customized version of
 *	LRESULT CALLBACK Window::WindowEventHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
 *	from OOGL/src/GL/Window/Window_Win32.cpp
 *	with ImGui event handler at the beginning
 */
LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

void LimitMaxFps(App::Config::WindowConfig window_config);

} // namespace App