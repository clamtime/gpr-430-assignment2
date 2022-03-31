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

		g += (amt * 2);
		if (g > 255)
			g = 0;

		b += (amt + 2);
		if (b > 255)
			b = 0;
	}

	void wrapColor()
	{
		if (r > 255)
			r = 0;

		if (g > 255)
			g = 0;

		if (b > 255)
			b = 0;
	}
};