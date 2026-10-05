#include "raylib.h"

#include "resource_dir.h"	// utility header for SearchAndSetResourceDir

// Types and Structures Definition
typedef enum GameScreen {LOGO = 0, TITLE, GAMEPLAY, GAMEOVER} GameScreen;
typedef enum Direction {UP = 0, DOWN, LEFT, RIGHT} Direction;

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Constants and Gameplay Changing Variables
	const int screenWidth = 800;
	const int screenHeight = 600;

	GameScreen currentScreen = LOGO;
	int frameCount = 0;
	bool gameoverTriggered = false;

	// For the "snake"
	Vector2 snakePos = {(float)screenWidth/2 - 16, (float)screenHeight/2 - 16};
	Vector2 snakeSize = {32.0f, 32.0f};
	const float speed = 250.0f;
	float speedVariable = 1.0f;
	float movingDirection = 1.0f;
	Direction lastDirection = RIGHT;

	// Create the window and OpenGL context
	InitWindow(screenWidth, screenHeight, "Not a Snake");
	SetTargetFPS(60);

	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	// Load a texture from the resources directory
	Texture wabbit = LoadTexture("wabbit_alpha.png");
	
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
				if (gameoverTriggered) 
				{
					currentScreen = GAMEOVER;
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
						snakePos.x += GetFrameTime() * (speed + speedVariable) * movingDirection;
					}
					else 
					{
						snakePos.y += GetFrameTime() * (speed + speedVariable) * movingDirection;
					}
					

					// No deaths yet
					if (snakePos.x > screenWidth) snakePos.x = 0 - 32;
					if (snakePos.x < 0 - 32) snakePos.x = screenWidth;
					if (snakePos.y > screenHeight) snakePos.y = 0 - 32;
					if (snakePos.y < 0 - 32) snakePos.y = screenHeight;

					//DrawTextureV(wabbit, snakePos, GRAY);
					DrawRectangleV(snakePos, snakeSize, GREEN);
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
	UnloadTexture(wabbit);

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}


