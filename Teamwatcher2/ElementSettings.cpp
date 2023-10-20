#include "ElementSettings.hpp"

namespace GUI {

	bool ElementSettings::is_set(const ElementAttributes& attribute) const
	{
		return m_flags[static_cast<size_t>(attribute)];
	}

	ElementSettings::operator DWORD() const
	{
		DWORD attr = 0;
		#define ADD_ATTR(name, value) attr |= is_set(ElementAttributes::name) ? value : attr;
		ELEMENT_ATTRIBUTES(ADD_ATTR)
		#undef	ADD_ATTR
			return attr;
	}

}