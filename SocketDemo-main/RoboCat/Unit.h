#pragma once


struct Vector2
{
	float x;
	float y;

	Vector2::Vector2() { x = 0; y = 0; };
	Vector2::Vector2(float a, float b) { x = a; y = b; };
};

class Unit
{
private:

	Vector2 position;

	// 0 - square, 1 - vRect, 2 - hRect
	int type;

public:

	Unit::Unit()
	{
		position = Vector2(0, 0);
		type = 0;
	}

	Unit::Unit(Vector2 pos, int t)
	{
		position = pos;
		type = t;
	}

	Vector2 getPosition() { return position; };
	void setPosition(Vector2 pos) { position = pos; };
	int getType() { return type; };
};