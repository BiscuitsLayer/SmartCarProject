#pragma once

/*
* Customized window class for Win32
* to set up customized event handler for Win32 window
* to be able to handle ImGui events
*/

// OpenGL Wrapper
#include <GL/Window/Window.hpp>

// ImGui
#include "imgui_impl_win32.h"
#include "imgui_internal.h"

// Forward declaration of event handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace App {

// Customized event handler forward declaration
LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

// This class only purpose is to have customized event handler as a friend function
class CustomWindow : public GL::Window {
	// Event handler now has access to window's private members
    friend LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
    CustomWindow(GL::uint width, GL::uint height, const std::string& title, GL::WindowStyle::window_style_t style) 
    : Window(width, height, title, style, WindowEventHandlerWithImGui) {
    }
};

// Customized version of 
// LRESULT CALLBACK Window::WindowEventHandler( HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam )
// from OOGL/src/GL/Window/Window_Win32.cpp
// with ImGui event handler at the beginning
LRESULT CALLBACK WindowEventHandlerWithImGui(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }

	CustomWindow* window;

	if ( msg == WM_NCCREATE )
	{
		// Store pointer to associated Window class as userdata in Win32 window
		window = reinterpret_cast<CustomWindow*>( ( (LPCREATESTRUCT)lParam )->lpCreateParams );
		window->window = hwnd;

		SetWindowLongPtr( hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( window ) );

		return DefWindowProc( hwnd, msg, wParam, lParam );
	} else {
		window = reinterpret_cast<CustomWindow*>( GetWindowLongPtr( hwnd, GWLP_USERDATA ) );
		
		if( window != nullptr )
			return window->WindowEvent( msg, wParam, lParam );
		else
			return DefWindowProc( hwnd, msg, wParam, lParam );
	}
}

} // namespace App