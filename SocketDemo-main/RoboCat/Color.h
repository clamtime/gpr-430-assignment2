#pragma once
#include <SDL_stdinc.h>

struct Color
{
	Uint8 r, g, b, a = 255;

	Color::Color()
	{
		r = g = b = a = 255;
	}

	Color::Color(Uint8 x, Uint8 y, Uint8 z, Uint8 w)
	{
		r = x;
		g = y;
		b = z;
		a = w;
	}

	void incrementColor(Uint8 amt = 1)
	{
		r += amt;
		if (r > 255)
			r = 0;

		g += amt;
		//g += (amt * 2);
		if (g > 255)
			g = 0;

		b += amt;
		//b += (amt + 2);
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