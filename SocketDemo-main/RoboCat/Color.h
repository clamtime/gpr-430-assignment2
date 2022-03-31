#pragma once
#include <SDL_stdinc.h>

struct Color
{
	Uint8 r, b, g, a = 255;

	void incrementColor(Uint8 amt = 1)
	{
		r += amt;
		if (r > 255)
			r = 0;

		g += amt;
		if (g > 255)
			g = 0;

		b += amt;
		if (b > 255)
			b = 0;
	}
};