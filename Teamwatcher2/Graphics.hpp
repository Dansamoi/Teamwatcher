#pragma once
#include <cstdint>

namespace Graphics {

	using color_value_t = uint8_t;

	/* Color structure */
	struct color_t {
		color_value_t red;
		color_value_t green;
		color_value_t blue;

		color_t(color_value_t red_value, color_value_t green_value, color_value_t blue_value) :
			red(red_value), green(green_value), blue(blue_value) { }
	};

};