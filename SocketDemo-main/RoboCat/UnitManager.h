#pragma once
#include "Unit.h"
#include "Color.h"
#include <vector>
#include <time.h>

class UnitManager
{
private:
	std::vector<Color> colors;

public:
	int count;
	std::vector<Unit> units;

	UnitManager::UnitManager()
	{
		std::srand(time(NULL));
		count = 0;
		createColors();
	}

	void createReceivedUnit(Vector2 _pos, Vector2 _size, Color _col, int _type, int _id);

	void createSquare(Vector2 screen);

	void createRectV(Vector2 screen);

	void createRectH(Vector2 screen);

	void createColors();

	void updateUnits(float dt, Vector2 screen);

	void RenderUnits(SDL_Renderer* renderer);

	int GetRandInt(int min, int max);

	Unit* getUnit(int id);
};
