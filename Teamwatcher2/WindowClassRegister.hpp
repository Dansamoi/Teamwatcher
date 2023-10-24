#pragma once
#include <unordered_map>
#include <string>
#include <mutex>

#include "WindowClass.h"


namespace GUI {

    /*
     * Class to register new window class
     */
    class WindowClassRegister final
    {
    public:
        /*
         * Method to get instance to class register
         */
        static WindowClassRegister& instance();

        /*
         * Registers window class
         * @param[in]   owner_instance  instance of the owner of the class
         * @param[in]   class_name      class's name
         * @param[in]   color           color of background
         * @param[in]   callback        callback function of the class
         * 
         * @return      true if added successfully, otherwise false (for example class already exists)
         */
        bool register_class(HINSTANCE owner_instance, const std::string& class_name, const Graphics::color_t& color, window_callback_t const& callback) noexcept;

        /*
         * Finds window class
         * @param[in]   class_name      class's name
         *
         * @return      true if found, otherwise false
         */
        bool contains_class(const std::string& class_name) noexcept;

        /*
         * Unregisters window class
         * @param[in]   class_name      class's name
         *
         * @return      true if added successfully, otherwise false (for example class not exists)
         */
        bool unregister_class(const std::string& class_name) noexcept;

    private:
        // should contain the HINSTANCE TODO

        WindowClassRegister() = default;

        std::mutex m_lock;
        std::unordered_map<std::string, WindowClass> m_registered_classes;
    };

}