#include "raylib.h"
#include <math.h>

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

#define GRID_CELL_SIZE 20
#define MAX_GRID_CELLS 36

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
} PlayerData;

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Constants and Gameplay Changing Variables
	const int screenWidth = 1280; // 1280
	const int screenHeight = 720; // 720

	int miniScreenSize = 720;
	int miniScreenStartX = screenWidth/2 - miniScreenSize/2;
	int miniScreenStartY = screenHeight/2 - miniScreenSize/2;

	GameScreen currentScreen = LOGO;
	int frameCount = 0;
	bool gameoverTriggered = false;
	float frameTimeCount = 0;

	// For the "snake"
	PlayerData player = { 0 };
	player.cell = (Point){ 2, 2 };
	player.color = GREEN;

	int snakeSize = 20; // pixels
	const float speed = 5.0f; // in frames per cell
	float speedVariable = 1.0f;
	float movingDirection = 1.0f;
	Direction lastDirection = RIGHT;

	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "Not a Snake");
	SetTargetFPS(120);

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	//Texture wabbit = LoadTexture("wabbit_alpha.png");
	
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		// For Handling Screen Changes
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
				frameTimeCount += GetFrameTime() * 60;

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

		// drawing
		BeginDrawing();

			// Setup the back buffer for drawing (clear color and depth buffers)
			ClearBackground(RAYWHITE);

			// For handling gameplay and logic
			switch (currentScreen)
			{
				case LOGO:
					//DrawTexture(wabbit, screenWidth/2 - 16, screenHeight/2 - 16, RED);
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
					// For detecting input
					if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A))
					{
						movingDirection = -1.0f;
						lastDirection = LEFT;
					}
					else if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D))
					{
						movingDirection = 1.0f;
						lastDirection = RIGHT;
					}
					else if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
					{
						movingDirection = -1.0f;
						lastDirection = UP;
					}
					else if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S))
					{
						movingDirection = 1.0f;
						lastDirection = DOWN;
					}

					// For handling "snake" direction movement
					if (lastDirection == RIGHT || lastDirection == LEFT)
					{
						if (frameTimeCount > speed) {
							player.cell.x += movingDirection;
							frameTimeCount = 0;
						}
						//player.cell.x += GetFrameTime() * movingDirection * speed;
						//snakePos.x += GetFrameTime() * (speed + speedVariable) * movingDirection;
					}
					else 
					{
						
						if (frameTimeCount > speed) {
							player.cell.y += movingDirection;
							frameTimeCount = 0;
						}
						//snakePos.y += GetFrameTime() * (speed + speedVariable) * movingDirection;
					}
					

					// No deaths yet
					if (player.cell.x < 0) player.cell.x = 0;
					else if (player.cell.x >= MAX_GRID_CELLS) player.cell.x = MAX_GRID_CELLS - 1;
					if (player.cell.y < 0) player.cell.y = 0;
					else if (player.cell.y >= MAX_GRID_CELLS) player.cell.y = MAX_GRID_CELLS - 1;


					// Drawing the grid and playable area
					DrawRectangle(miniScreenStartX, miniScreenStartY, miniScreenSize, miniScreenSize, RAYWHITE);
					
					for (int y = 0; y <= MAX_GRID_CELLS; y++)
					{
						DrawLine(miniScreenStartX, miniScreenStartY + y*GRID_CELL_SIZE, miniScreenStartX + MAX_GRID_CELLS*GRID_CELL_SIZE, miniScreenStartY + y*GRID_CELL_SIZE, LIGHTGRAY);
					}

					for (int x = 0; x <= MAX_GRID_CELLS; x++)
					{
						DrawLine(miniScreenStartX + x*GRID_CELL_SIZE, miniScreenStartY, miniScreenStartX + x*GRID_CELL_SIZE, miniScreenStartY + MAX_GRID_CELLS*GRID_CELL_SIZE, LIGHTGRAY);
					}
					
					//DrawTextureV(wabbit, snakePos, GRAY);
					DrawRectangle(miniScreenStartX + player.cell.x*GRID_CELL_SIZE, miniScreenStartY + player.cell.y*GRID_CELL_SIZE, snakeSize, snakeSize, GREEN);

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


