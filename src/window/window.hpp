#pragma once

// STL
#include <thread>

// OpenGL Wrapper
#include <GL/Window/Window.hpp>

#ifdef _WIN32
// ImGui
#include "imgui_impl_win32.h"
#include "imgui_internal.h"
#endif

// Constants
#include <constants/constants.hpp>

// Forward declarations
#include <window/window_fwd.hpp>

// // LibSmartCar
// #include <config/config_handler.hpp>

// Incomplete type resolve
#include <config/config_handler_fwd.hpp>

#ifdef _WIN32
// Forward declaration of event handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

namespace App {

#ifdef _WIN32
// Customized event handler forward declaration
LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

// The only purpose of that class is to have customized event handler as a friend function
class CustomWindow: public GL::Window {
	/*
	 *	Customized window class for Win32
	 *	to set up customized event handler for Win32 window
	 *	to be able to handle ImGui events
	 */
public:
	CustomWindow(GL::uint width, GL::uint height, const std::string& title, GL::WindowStyle::window_style_t style);
	CustomWindow(const Config::WindowConfig& config);

private:
#ifdef _WIN32
	// Event handler now has access to window's private members
	friend LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif
};

#ifdef _WIN32
/*
 *	Customized version of
 *	LRESULT CALLBACK Window::WindowEventHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
 *	from OOGL/src/GL/Window/Window_Win32.cpp
 *	with ImGui event handler at the beginning
 */
LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
#endif

void LimitMaxFps(const Config::WindowConfig& window_config);

} // namespace App
