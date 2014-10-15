#include <HAPI_lib.h>
#include <Windows.h>
#include <iostream>	
#include <time.h>
#include <string>

struct Star{
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;
	HAPI_TColour colour;
	float speed = 1.0f;

	Star::Star(float x_, float y_, float z_, float speed_, HAPI_TColour colour_) : x(x_), y(y_), z(z_), speed(speed_), colour(colour_){}
	Star::Star(){}
};

const HAPI_TColour COL_WHITE = HAPI_TColour(255, 255, 255);

int screenWidth = 1600;
int screenHeight = 700;

BYTE* screenStart = nullptr;

Star** stars = new Star*[100];

void clearScreen(HAPI_TColour& colour)
{
	if (colour.red == 0 && colour.green == 0 && colour.blue == 0)
	{
		memset(screenStart, 0, screenWidth * screenHeight * 4);
	}

	for (int offset = 0; offset < screenWidth * screenHeight; offset++)
	{
		memcpy(screenStart + (offset * 4), &colour, 4);
	}
}

inline void setPixel(int x, int y, HAPI_TColour& colour)
{
	if (x < 0 || x > screenWidth || y < 0 || y > screenHeight)
	{
		return;
	}

	int offset = (x + y * screenWidth) * 4;
	memcpy(screenStart + offset, &colour, 4);
}

float eyeDist = 1;

inline void setPixel3D(int x, int y, int z, HAPI_TColour colour)
{
	int cX = screenWidth / 2;
	int cY = screenHeight / 2;
	int sX = ((eyeDist * (x - cX)) / (eyeDist + z)) + cX;
	int sY = ((eyeDist * (y - cY)) / (eyeDist + z)) + cY;
	setPixel(sX, sY, colour);
}

void drawRect3D(int x, int y, int z, int width, int height, HAPI_TColour& colour)
{
	int cX = screenWidth / 2;
	int cY = screenHeight / 2;
	int sX = ((eyeDist * (x - cX)) / (eyeDist + z)) + cX;
	int sY = ((eyeDist * (y - cY)) / (eyeDist + z)) + cY;

	if (sX + width > screenWidth || sY + height > screenHeight)
	{
		return;
	}

	sX -= width / 2;
	sY -= height / 2;

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			setPixel(sX + i, sY + j, colour);
		}
	}
}

void drawStar(Star* star)
{
	if (star->z < eyeDist)
	{
		return;
	}
	int size = 100/(star->z - eyeDist);
	drawRect3D(star->x, star->y, star->z, size, size, star->colour);
}

Star* createStar(){
	float x = -screenWidth / 2 + rand() % screenWidth * 2;
	float y = -screenHeight / 2 + rand() % screenHeight * 2;
	float z = 100;
	float speed = 0.1 + ((rand() % 100) / 100.f);

	return new Star(x, y, z, speed, HAPI_TColour(COL_WHITE));
}

bool moving = false;

void updateStars()
{
	for (int i = 0; i < 100; i++)
	{
		Star* star = stars[i];
		if (moving)
			star->z -= star->speed;

		int dist = star->z - eyeDist;
		if (dist <= 0)
		{
			delete stars[i];
			stars[i] = createStar();
		}

		drawStar(star);
	}
}

void updateInput()
{
	HAPI_TMouseData* mDat = new HAPI_TMouseData();
	HAPI->GetMouseData(mDat);
	if (mDat->wheelMovement < 0)
	{
		eyeDist -= 1;
	}
	if (mDat->wheelMovement > 0)
	{
		eyeDist += 1;
	}

	moving = true;

	HAPI_TKeyboardData* kDat = new HAPI_TKeyboardData();
	HAPI->GetKeyboardData(kDat);
	if (kDat->scanCode[' '])
		moving = true;
}

void HAPI_Main()
{
	if (!HAPI->Initialise(&screenWidth, &screenHeight))
		return;

	screenStart = HAPI->GetScreenPointer();
	HAPI->SetShowFPS(true);	
	srand(time(NULL));
	for (int i = 0; i < 100; i++){
		int x = -screenWidth/2 + rand() % screenWidth * 2;
		int y = rand() % screenHeight;
		int z = rand() % 100;
		stars[i] = createStar();
	}

	while (HAPI->Update())
	{
		clearScreen(HAPI_TColour(50, 50, 50));

		updateStars();
		updateInput();
	}
}