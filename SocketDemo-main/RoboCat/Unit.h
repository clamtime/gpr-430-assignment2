#pragma once
#include "Color.h"
#include "SDL.h"

struct Vector2
{
	int x;
	int y;

	Vector2::Vector2() { x = 0; y = 0; };
	Vector2::Vector2(float a, float b) { x = a; y = b; };
};

class Unit
{
private:

	Vector2 position;
	Vector2 size; // width + height
	Color color;

	SDL_Rect rect;

	// 0 - square, 1 - vRect, 2 - hRect
	int type;

	bool right;

public:

	Unit::Unit()
	{
		position = Vector2(0, 0);
		size = Vector2(1, 1);
		type = 0;

		initRect();
		right = true;
	}

	Unit::Unit(Vector2 pos, Vector2 s, int t, Color col)
	{
		position = pos;
		size = s;
		type = t;
		color = col;

		initRect();
		right = true;
	}

	void initRect()
	{
		rect.x = position.x;
		rect.y = position.y;
		rect.w = size.x;
		rect.h = size.y;
	}

	Vector2 getPosition() { return position; };
	void setPosition(Vector2 pos) { position = pos; rect.x = position.x; rect.y = position.y; };
	int getType() { return type; };
	SDL_Rect getRect() { return rect; };

	void update(float dt, Vector2 screen)
	{
		int gravity = 4;

		switch (type)
		{
		case 0:
			// square - cycle through colors
			color.incrementColor();
			break;

		case 1:
			// vRect - fall
			if (position.y >= screen.y)
			{
				position.y = -size.y;
			}
			position.y += gravity;
			rect.y = position.y;
			break;

		case 2:
			// hRect - teleport
			if (position.x <= 0)
				right = true;
			else if (position.x + size.x>= screen.x)
				right = false;

			if (right)
				position.x += 3;
			else
				position.x -= 3;

			rect.x = position.x;

			break;
		}
	}

	void render(SDL_Renderer* renderer)
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		// draw rect
		SDL_RenderFillRect(renderer, &rect);

		SDL_RenderPresent(renderer);
	}
};