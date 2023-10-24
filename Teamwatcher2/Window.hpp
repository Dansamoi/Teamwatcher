#pragma once
#include "ElementSettings.hpp"
#include "Grid.hpp"

#include <Windows.h>
#include <string>

namespace GUI {

	/*
	 * Window state
	 */
	enum class WindowState : uint8_t {
		VISIBLE		= SW_SHOW,
		INVISIBLE	= SW_HIDE,
		MAXIMIZE	= SW_MAXIMIZE,
		MINIMIZE	= SW_MINIMIZE
	};

	/*
	 * Window class to handle window
	 */
	class Window final
	{
	public:
		/**
		 * Window ctor
		 * 
		 * @param[in]		owner_instance		instance of owner
		 * @param[in]		class_name			name of class
		 * @param[in]		window_text			text of window
		 * @param[optional]	width				width of window					
		 * @param[optional]	height				height of window
		 */
		Window(HINSTANCE owner_instance, 
			const std::string& class_name, 
			const std::string& window_text, 
			const WindowSettings& settings,
			size_t width = DEFAULT_WINDOW_WIDTH, 
			size_t height = DEFAULT_WINDOW_HEIGHT);

		/*
		 * Method to change window state
		 * 
		 * @param[in]		new_state			state to change to
		 * 
		 * @return success status
		 */
		bool change_state(const WindowState& new_state);

		/*
		 * Method to redraw window
		 */
		bool update();

		/**
		 * Window dtor
		 */
		~Window();

		/*
		 * Deleted move/copy assignment
		 */
		Window(Window& other) = delete;
		Window& operator=(Window& other) = delete;
		Window(Window&& other) = delete;
		Window& operator=(Window&& other) = delete;

	private:
		static constexpr size_t DEFAULT_WINDOW_WIDTH = 800;
		static constexpr size_t DEFAULT_WINDOW_HEIGHT = 450;

		/**
		 * Method to get dimensions of window
		 * 
		 * @param[out]	o_dimensions	dimensions of window
		 * 
		 * @returns success status
		 */
		bool get_dimensions(GUI::Dimensions& o_dimensions);

		Grid m_grid;

		HWND m_window;
		std::string m_class_name;
		std::string m_text;
	};

}