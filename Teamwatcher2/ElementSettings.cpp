#include "ElementSettings.hpp"

namespace GUI {

	template<typename AttributesEnum>
	bool Attributes<AttributesEnum>::is_set(const AttributesEnum& attribute) const {
		return m_flags[static_cast<size_t>(attribute)];
	}

	template<typename AttributesEnum>
	Attributes<AttributesEnum>::operator DWORD() const {
		static_assert(false, "Enum not of attributes");
	}
	
#define ADD_ATTR(name, value)	attr |= is_set(EnumName::name) ? value : 0;
#define GENERATE_CAST_FUNC(attr_list, enum_name)							\
	enum_name ## Settings::operator DWORD() const {							\
		DWORD attr = 0;														\
		{																	\
			using EnumName = enum_name ## Attributes;						\
			attr_list(ADD_ATTR)												\
		}																	\
		return attr;														\
	}

	ATTRIBUTE_LISTS(GENERATE_CAST_FUNC)

#undef	ADD_ATTR
#undef GENERATE_COUNT_FUNCS

}