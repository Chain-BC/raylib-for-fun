#include "raylib.h"
#include "raymath.h"
#include <math.h>

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#define MAX_GRID_CELLS 25

// Types and Structures Definition
typedef enum GameScreen {LOGO = 0, TITLE, GAMEPLAY, GAMEOVER} GameScreen;
typedef enum Direction {UP = 0, DOWN, LEFT, RIGHT} Direction;

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	Point cell;
	Color color;
	int size;
} PlayerData;

typedef struct {
	Point cell;
	Color color;
	int size;
} FruitData;

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 720, "Not a Snake");

	int monitor = GetCurrentMonitor();
	int monitorWidth = GetMonitorWidth(monitor);
	int monitorHeight = GetMonitorHeight(monitor);

	SetWindowSize(monitorWidth, monitorHeight);
	ToggleFullscreen();
	SetTargetFPS(60);

	// Constants and Gameplay Changing Variables
	const int screenWidth = GetScreenWidth();
	const int screenHeight = GetScreenHeight();

	int miniScreenSize = screenHeight;
	int miniScreenStartX = screenWidth/2 - miniScreenSize/2;
	int miniScreenStartY = screenHeight/2 - miniScreenSize/2;
	const int gridCellSize = miniScreenSize / MAX_GRID_CELLS;

	GameScreen currentScreen = LOGO;
	int frameCount = 0;
	bool gameoverTriggered = false;
	float frameTimeCount = 0;

	// For the "snake"
	PlayerData player = { 0 };
	player.cell = (Point){ 2, 2 };
	player.color = GREEN;
	player.size = gridCellSize;

	FruitData fruit = { 0 };
	fruit.cell = (Point){ GetRandomValue(0, MAX_GRID_CELLS - 1), GetRandomValue(0, MAX_GRID_CELLS - 1)};
	fruit.color = RED;
	fruit.size = gridCellSize/3;

	const float speed = 10.0f; // in frames per cell
	float speedVariable = 0.0f; // subtracted from speed to make the snake go faster over time
	float movingDirection = 1.0f;
	Direction lastDirection = RIGHT;

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		// 
		// Screen management and transitions
		//
		switch (currentScreen)
		{
			
			case LOGO:
				frameCount++;

				if (frameCount > 120)
				{
					currentScreen = TITLE;
				}
				break;

			case TITLE:
				if (IsKeyPressed(KEY_SPACE)) 
				{
					currentScreen = GAMEPLAY;
				}
				break;

			case GAMEPLAY:
				if (gameoverTriggered) 
				{
					currentScreen = GAMEOVER;
					frameTimeCount = 0;
				}
				break;

			case GAMEOVER:
				if (IsKeyPressed(KEY_SPACE)) 
				{
					currentScreen = TITLE;
				}
				
			default: break;
		}

		// 
		// Gameplay logic and input handling
		//
		if (currentScreen == GAMEPLAY)
		{
			frameTimeCount += GetFrameTime() * 60;

			// Check if the player has eaten the fruit
			if (player.cell.x == fruit.cell.x && player.cell.y == fruit.cell.y)
			{
				fruit.cell.x = GetRandomValue(0, MAX_GRID_CELLS - 1);
				fruit.cell.y = GetRandomValue(0, MAX_GRID_CELLS - 1);
				speedVariable += 0.15f;
			}

			// For detecting input
			if ((IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) && lastDirection != RIGHT)
			{
				movingDirection = -1.0f;
				lastDirection = LEFT;
			}
			else if ((IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) && lastDirection != LEFT)
			{
				movingDirection = 1.0f;
				lastDirection = RIGHT;
			}
			else if ((IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W)) && lastDirection != DOWN)
			{
				movingDirection = -1.0f;
				lastDirection = UP;
			}
			else if ((IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) && lastDirection != UP)
			{
				movingDirection = 1.0f;
				lastDirection = DOWN;
			}

			// For handling "snake" direction movement
			float moveInterval = Clamp(speed - speedVariable, 2.0f, speed);
			if (lastDirection == RIGHT || lastDirection == LEFT)
			{
				if (frameTimeCount >= moveInterval) {
					player.cell.x += movingDirection;
					frameTimeCount = 0;
				}
			}
			else 
			{
				if (frameTimeCount >= moveInterval) {
					player.cell.y += movingDirection;
					frameTimeCount = 0;
				}
			}
			

			// No deaths yet
			if (player.cell.x < 0) player.cell.x = 0;
			else if (player.cell.x >= MAX_GRID_CELLS) player.cell.x = MAX_GRID_CELLS - 1;
			if (player.cell.y < 0) player.cell.y = 0;
			else if (player.cell.y >= MAX_GRID_CELLS) player.cell.y = MAX_GRID_CELLS - 1;
		}

		// 
		// Everything drawing
		//
		BeginDrawing();

			// Setup the back buffer for drawing (clear color and depth buffers)
			ClearBackground(RAYWHITE);

			// For handling gameplay and logic
			switch (currentScreen)
			{
				case LOGO:
					DrawRectangle(screenWidth/2 - 128, screenHeight/2 - 128, 256, 256, BLACK);
					DrawRectangle(screenWidth/2 - 112, screenHeight/2 - 112, 224, 224, RAYWHITE);
					DrawText("raylib", screenWidth/2 - 44, screenHeight/2 + 48, 50, BLACK);
					break;
				
				case TITLE:
					DrawRectangle(screenWidth/10, screenHeight/10, screenWidth * 4/5, screenHeight * 4/5, BLACK);
					DrawText("PRESS SPACE TO PLAY!", screenWidth/2, screenHeight/2, 1, RAYWHITE);
					break;

				case GAMEPLAY:
					ClearBackground(BLACK);

					// Drawing the grid and playable area
					DrawRectangle(miniScreenStartX, miniScreenStartY, miniScreenSize, miniScreenSize, RAYWHITE);
					
					for (int y = 0; y <= MAX_GRID_CELLS; y++)
					{
						DrawLine(miniScreenStartX, miniScreenStartY + y*gridCellSize, miniScreenStartX + MAX_GRID_CELLS*gridCellSize, miniScreenStartY + y*gridCellSize, LIGHTGRAY);
					}

					for (int x = 0; x <= MAX_GRID_CELLS; x++)
					{
						DrawLine(miniScreenStartX + x*gridCellSize, miniScreenStartY, miniScreenStartX + x*gridCellSize, miniScreenStartY + MAX_GRID_CELLS*gridCellSize, LIGHTGRAY);
					}
					// Fruit
					DrawCircle(miniScreenStartX + fruit.cell.x*gridCellSize + gridCellSize/2, miniScreenStartY + fruit.cell.y*gridCellSize + gridCellSize/2, fruit.size, fruit.color);

					// Snake
					DrawRectangle(miniScreenStartX + player.cell.x*gridCellSize, miniScreenStartY + player.cell.y*gridCellSize, player.size, player.size, GREEN);
					
					break;

				case GAMEOVER:

					break;
				default: break;
			}
		
		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}

	// cleanup
	// unload our texture so it can be cleaned up
	//UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}


