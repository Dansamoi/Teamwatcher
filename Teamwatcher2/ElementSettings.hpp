#pragma once
#include <bitset>
#include <Windows.h>

namespace GUI {

#define ELEMENT_ATTRIBUTES(X)						\
	X(WITH_BORDER,			WS_BORDER		)		\
	X(WITH_CAPTION,			WS_CAPTION		)		\
	X(IS_CHILD,				WS_CHILD		)		\
	X(CLIP_CHILDREN,		WS_CLIPCHILDREN	)		\
	X(CLIP_SIBLINGS,		WS_CLIPSIBLINGS	)		\
	X(DISABLED,				WS_DISABLED		)		\
	X(DIALOG_FRAME,			WS_DLGFRAME		)		\
	X(HORIZONTAL_SCROLL,	WS_HSCROLL		)		\
	X(MAXIMIZED,			WS_MAXIMIZE		)		\
	X(CAN_MAXIMIZE,			WS_MAXIMIZEBOX	)		\
	X(MINIMIZED,			WS_MINIMIZE		)		\
	X(CAN_MINIMIZE,			WS_MINIMIZEBOX	)		\
	X(IS_OVERLAPPED,		WS_OVERLAPPED	)		\
	X(IS_POPUP,				WS_POPUP		)		\
	X(RESIZABLE,			WS_SIZEBOX		)		\
	X(HAS_SYSMENU,			WS_SYSMENU		)		\
	X(TAB_FOCUS,			WS_TABSTOP		)		\
	X(VISIBLE,				WS_VISIBLE		)		\
	X(VERTICAL_SCROLL,		WS_VSCROLL		)		\

	constexpr static size_t ElementAttributesCount() {
		size_t count = 0;
		#define INC_COUNT(name, value) ++count;
		ELEMENT_ATTRIBUTES(INC_COUNT)
		#undef INC_COUNT
		return count;
	}

	enum class ElementAttributes : uint8_t {
		#define DEFINE_ELEMENT(name, value) name,
		ELEMENT_ATTRIBUTES(DEFINE_ELEMENT)
		#undef DEFINE_ELEMENT
	};

	/*
	 * Class for Setting for class
	 */
	class ElementSettings {
	public:
		template<typename... Attributes>
		constexpr ElementSettings(Attributes... attributes);

		constexpr void set_attributes() {}
		constexpr void unset_attributes() {}

		/*
		 * Sets attributes in settings
		 */
		template<typename... Attributes>
		constexpr void set_attributes(ElementAttributes attribute, Attributes... attributes);

		/*
		 * Unsets attributes in settings
		 */
		template<typename... Attributes>
		constexpr void unset_attributes(ElementAttributes attribute, Attributes... attributes);

		/*
		 * Checks if attribute is set
		 */
		bool is_set(const ElementAttributes& attribute) const;

		/*
		 * Return value for winapi 
		 */
		operator DWORD() const;

	private:
		std::bitset<ElementAttributesCount()> m_flags;
	};

	template<typename ...Attributes>
	constexpr inline ElementSettings::ElementSettings(Attributes... attributes)
	{
		m_flags.reset();
		set_attributes(attributes...);
	}

	template<typename ...Attributes>
	constexpr inline void ElementSettings::set_attributes(ElementAttributes attribute, Attributes ...attributes)
	{
		static_assert(std::is_same<ElementAttributes, typename std::remove_reference<decltype(attribute)>::type>::value,
			"All arguments must be of type ElementAttributes");

		m_flags[static_cast<size_t>(attribute)] = true;
		set_attributes(attributes...);
	}

	template<typename ...Attributes>
	inline constexpr void ElementSettings::unset_attributes(ElementAttributes attribute, Attributes ...attributes)
	{
		static_assert(std::is_same<ElementAttributes, typename std::remove_reference<decltype(attribute)>::type>::value,
			"All arguments must be of type ElementAttributes");

		m_flags[static_cast<size_t>(attribute)] = false;
		unset_attributes(attributes...);
	}

}