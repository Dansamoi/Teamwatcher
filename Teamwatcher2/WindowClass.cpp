#include "WindowClass.h"
#include "Resource.h"
#include <stdexcept>

GUI::WindowClass::WindowClass(HINSTANCE current_instance, const std::string& class_name, const Graphics::color_t& color, const window_callback_t& callback) : 
	m_window_class(),
	m_current_instance(current_instance), 
	m_class_name(class_name)
{
	m_window_class.hInstance = m_current_instance;
	m_window_class.lpszClassName = m_class_name.data();
	m_window_class.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	m_window_class.hbrBackground = CreateSolidBrush(RGB(color.red, color.green, color.blue));
	m_window_class.hIcon = LoadIconA(current_instance, MAKEINTRESOURCEA(IDI_TEAMWATCHER2)); // TODO: get icon name as string
	m_window_class.lpfnWndProc = *(callback.target<WNDPROC>());

	if (! RegisterClassA(&m_window_class)) {
		throw std::runtime_error("Failed to register class");
	}
}

GUI::WindowClass::~WindowClass()
{
	if (! UnregisterClassA(m_class_name.data(), m_current_instance)) {
		// TODO
	}
}
