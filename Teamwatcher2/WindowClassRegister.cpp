#include "WindowClassRegister.hpp"
#include <stdexcept>

namespace GUI {

    WindowClassRegister& WindowClassRegister::instance()
    {
        static WindowClassRegister instance;
        return instance;
    }

    bool WindowClassRegister::register_class(HINSTANCE owner_instance, const std::string& class_name, const Graphics::color_t& color, window_callback_t const& callback) noexcept
    {
        const std::lock_guard<std::mutex> lock(m_lock);

        if (m_registered_classes.end() != m_registered_classes.find(class_name)) {
            return false;
        }

        try {
            m_registered_classes.emplace(
                std::piecewise_construct,
                std::forward_as_tuple(class_name), 
                std::forward_as_tuple(owner_instance, class_name, color, callback)
            );
        }
        catch (const std::runtime_error&) {
            return false;
        }

        return true;
    }

    bool WindowClassRegister::contains_class(const std::string& class_name) noexcept
    {
        const std::lock_guard<std::mutex> lock(m_lock);

        return m_registered_classes.end() != m_registered_classes.find(class_name);
    }

    bool WindowClassRegister::unregister_class(const std::string& class_name) noexcept
    {
        const std::lock_guard<std::mutex> lock(m_lock);

        auto class_element = m_registered_classes.find(class_name);

        if (m_registered_classes.end() == class_element) {
            return false;
        }

        m_registered_classes.erase(class_element);

        return true;
    }

}