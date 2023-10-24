#pragma once
#include <windows.h>
#include <string>
#include <functional>

#include "Graphics.hpp"

namespace GUI {

	using window_callback_t = std::function<LRESULT CALLBACK(HWND, UINT, WPARAM, LPARAM)>;

	class WindowClass final
	{
	public:
		/*
		 * WindowClass ctor
		 */
		WindowClass(HINSTANCE current_instance, const std::string& class_name, const Graphics::color_t& color, window_callback_t const& callback);

		/*
		 * WindowClass ctor
		 */
		~WindowClass();

		/*
		 * Deleted move/copy ctor and assignment
		 */
		WindowClass(WindowClass& other) = delete;
		WindowClass& operator=(WindowClass& other) = delete;
		WindowClass(WindowClass&& other) = delete;
		WindowClass& operator=(WindowClass&& other) = delete;

	private:
		WNDCLASSA m_window_class;
		HINSTANCE m_current_instance;
		const std::string m_class_name;
	};

};
