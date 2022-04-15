#include "UnitManager.h"

void UnitManager::createSquare(Vector2 screen)
{
	int posx = GetRandInt(0, screen.x);
	int posy = GetRandInt(0, screen.y);
	int size = GetRandInt(20, 121);
	Color col = colors.at(GetRandInt(0, colors.size()));

	Unit* temp = &Unit(Vector2(posx, posy), Vector2(size, size), 0, col, count);
	units.push_back(temp);
	count++;
}

void UnitManager::createRectV(Vector2 screen)
{
	int posx = GetRandInt(0, screen.x);
	int posy = GetRandInt(0, screen.y);
	int sizex = GetRandInt(20, 40);
	int sizey = GetRandInt(40, 61);
	Color col = colors.at(GetRandInt(0, colors.size()));

	Unit* temp = &Unit(Vector2(posx, posy), Vector2(sizex, sizey), 1, col, count);
	units.push_back(temp);
	count++;
}

void UnitManager::createRectH(Vector2 screen)
{
	int posx = GetRandInt(0, screen.x);
	int posy = GetRandInt(0, screen.y);
	int sizex = GetRandInt(60, 81);
	int sizey = GetRandInt(40, 60);
	Color col = colors.at(GetRandInt(0, colors.size()));

	Unit* temp = &Unit(Vector2(posx, posy), Vector2(sizex, sizey), 0, col, count);
	units.push_back(temp);
	count++;
}

void UnitManager::createColors()
{
	colors.push_back(Color(200, 75, 60, 255));
	colors.push_back(Color(61, 226, 255, 255));
	colors.push_back(Color(155, 132, 245, 255));
	colors.push_back(Color(255, 168, 197, 255));
	colors.push_back(Color(12, 12, 179, 255));
	colors.push_back(Color(252, 0, 93, 255));
	colors.push_back(Color(255, 91, 15, 255));
	colors.push_back(Color(76, 199, 74, 255));
}

void UnitManager::updateUnits(float dt, Vector2 screen)
{
	for (int i = 0; i < count; i++)
		units.at(i)->update(dt, screen);
}

void UnitManager::RenderUnits(SDL_Renderer* renderer)
{
	for (int i = 0; i < count; i++)
		units.at(i)->render(renderer);
}

int UnitManager::GetRandInt(int min, int max)
{
	int r = std::rand() % max + min;
}