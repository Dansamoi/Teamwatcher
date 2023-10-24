#pragma once
#include <bitset>
#include <Windows.h>

namespace GUI {

// Attributes to any element
#define ELEMENT_ATTRIBUTES(X)						\
	X(CLIP_CHILDREN,		WS_CLIPCHILDREN	)		\
	X(CLIP_SIBLINGS,		WS_CLIPSIBLINGS	)		\
	X(DISABLED,				WS_DISABLED		)		\
	X(VISIBLE,				WS_VISIBLE		)		\

// Attributes for window
#define WINDOW_ATTRIBUTES(X)						\
	ELEMENT_ATTRIBUTES(X)							\
	X(WITH_BORDER,			WS_BORDER		)		\
	X(DIALOG_FRAME,			WS_DLGFRAME		)		\
	X(IS_OVERLAPPED,		WS_OVERLAPPED	)		\
	X(HAS_SYSMENU,			WS_SYSMENU		)		\
	X(WITH_CAPTION,			WS_CAPTION		)		\
	X(HORIZONTAL_SCROLL,	WS_HSCROLL		)		\
	X(MAXIMIZED,			WS_MAXIMIZE		)		\
	X(MAXIMIZABLE,			WS_MAXIMIZEBOX	)		\
	X(MINIMIZED,			WS_MINIMIZE		)		\
	X(MINIMIZABLE,			WS_MINIMIZEBOX	)		\
	X(VERTICAL_SCROLL,		WS_VSCROLL		)		\
	X(RESIZABLE,			WS_SIZEBOX		)		\
	//X(POPUP,				WS_POPUP		)		\

// Attributes for UI elements
#define UI_ATTRIBUTES(X)							\
	ELEMENT_ATTRIBUTES(X)							\
	X(TAB_FOCUS,			WS_TABSTOP		)		\


#define ATTRIBUTE_LISTS(X)							\
	X(WINDOW_ATTRIBUTES,	Window)					\
	X(UI_ATTRIBUTES,		Element	)				\
	// X(ELEMENT_ATTRIBUTES, General)


	/*
	 * Generate enums
	 */
#define DEFINE_ELEMENT(name, value) name,
#define GENERATE_ENUM(attr_list, enum_name)					\
	enum class enum_name ## Attributes : uint8_t {			\
		attr_list(DEFINE_ELEMENT)							\
	};

	ATTRIBUTE_LISTS(GENERATE_ENUM)
	
#undef DEFINE_ELEMENT
#undef GENERATE_ENUM

	/*
	 * Counts for lists
	 */
	template<typename Enum>
	constexpr static size_t AttributesCount() {
		static_assert(false, "Enum not of attributes");
	}

#define INC_COUNT(name, value) ++count;
#define GENERATE_COUNT_FUNC(attr_list, enum_name)							\
	template<>																\
	constexpr static size_t AttributesCount<enum_name ## Attributes>() {	\
		size_t count = 0;													\
		attr_list(INC_COUNT)												\
		return count;														\
	}

	ATTRIBUTE_LISTS(GENERATE_COUNT_FUNC)

#undef INC_COUNT
#undef GENERATE_COUNT_FUNCS

	/*
	 * Class for Setting for element
	 */
	template<typename AttributesEnum>
	class Attributes {
	public:
		template<typename... Attr>
		constexpr Attributes(Attr... attributes) {
			m_flags.reset();
			set_attributes(attributes...);
		}

		constexpr void set_attributes() {}
		constexpr void unset_attributes() {}

		/*
		 * Sets attributes in settings
		 */
		template<typename... Attr>
		constexpr void set_attributes(AttributesEnum attribute, Attr... attributes ) {
			static_assert(std::is_same<AttributesEnum, typename std::remove_reference<decltype(attribute)>::type>::value,
				"All arguments must be from the enum");

			m_flags[static_cast<size_t>(attribute)] = true;
			set_attributes(attributes...);
		}

		/*
		 * Unsets attributes in settings
		 */
		template<typename... Attr>
		constexpr void unset_attributes(AttributesEnum attribute, Attr... attributes) {
			static_assert(std::is_same<AttributesEnum, typename std::remove_reference<decltype(attribute)>::type>::value,
				"All arguments must be from the enum");

			m_flags[static_cast<size_t>(attribute)] = false;
			unset_attributes(attributes...);
		}

		/*
		 * Checks if attribute is set
		 */
		bool is_set(const AttributesEnum& attribute) const;

		/*
		 * Return value for winapi 
		 */
		operator DWORD() const;

	private:
		std::bitset<AttributesCount<AttributesEnum>()> m_flags;
	};

#define DEFINE_TYPE(attr_list, enum_name)	using enum_name ## Settings = Attributes<enum_name ## Attributes>;
	ATTRIBUTE_LISTS(DEFINE_TYPE)
#undef DEFINE_TYPE
}