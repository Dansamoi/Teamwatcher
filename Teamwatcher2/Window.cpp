#include "Window.hpp"
#include <stdexcept>

GUI::Window::Window(
	HINSTANCE owner_instance, 
	const std::string& class_name, 
	const std::string& window_text, 
	const WindowSettings& settings,
	size_t width, size_t height) : m_class_name(class_name), m_text(window_text)
{
	m_window = CreateWindowExA(
		0,												// Optional window styles.
		m_class_name.data(),								// Window class
		m_text.data(),								// Window text
		settings,	//WS_OVERLAPPEDWINDOW           // Window style
		
		// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, width, height,
		
		nullptr,			// Parent window    
		nullptr,			// Menu
		owner_instance,		// Instance handle
		nullptr				// Additional application data
	);

	if (nullptr == m_window) {
		throw std::runtime_error("failed to create window");
	}

	if (! change_state(WindowState::VISIBLE)) {
		throw std::runtime_error("failed to change window state");
	}
}

bool GUI::Window::change_state(const WindowState& new_state)
{
	ShowWindow(m_window, static_cast<uint8_t>(new_state));

	return UpdateWindow(m_window);
}

bool GUI::Window::update()
{
	GUI::Dimensions current_dimensions;

	if (!get_dimensions(current_dimensions)) {
		return false;
	}

	return m_grid.update(current_dimensions);
}

GUI::Window::~Window()
{
	if (! DestroyWindow(m_window)) {
		// TODO
	}
}

bool GUI::Window::get_dimensions(GUI::Dimensions& o_dimensions)
{
	RECT rect;

	if (! GetWindowRect(m_window, &rect)) {
		return false;
	}

	o_dimensions.width = rect.right - rect.left;
	o_dimensions.height = rect.bottom - rect.top;

	return true;
}
