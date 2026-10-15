#include "raylib.h"
#include "raymath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


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

typedef struct {
	size_t size;
	size_t capacity;
	PlayerData* array;
} snake_array;

// function prototypes
void InitSnakeArray(snake_array** arr_ptr);
void FreeSnakeArray(snake_array* container);

void InsertSegment(snake_array* container, PlayerData data);
void UpdateSegment(snake_array* container, int index, PlayerData data);
PlayerData GetSegment(snake_array* container, int index);

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

	snake_array* snakeSegments;
	InitSnakeArray(&snakeSegments);
	InsertSegment(snakeSegments, player);

	const float speed = 10.0f; // in frames per cell
	float speedVariable = 0.0f; // subtracted from speed to make the snake go faster over time
	float movingDirection = 1.0f;
	Direction lastDirection = RIGHT;
	bool hasMoved = false;

	// For the fruit
	FruitData fruit = { 0 };
	fruit.cell = (Point){ GetRandomValue(0, MAX_GRID_CELLS - 1), GetRandomValue(0, MAX_GRID_CELLS - 1)};
	fruit.color = RED;
	fruit.size = gridCellSize/3;

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

			// For handling new position of the head segment
			float moveInterval = Clamp(speed - speedVariable, 2.0f, speed);
			if (frameTimeCount >= moveInterval) {
				PlayerData snakeHead = GetSegment(snakeSegments, 0);

				// Check if the player has eaten the fruit
				if (snakeHead.cell.x == fruit.cell.x && snakeHead.cell.y == fruit.cell.y)
				{
					// Necessary so the fruit doesn't spawn on the snake
					for (int i = 0; i < snakeSegments->size; i++)
					{
						PlayerData segment = GetSegment(snakeSegments, i);
						if (segment.cell.x == fruit.cell.x && segment.cell.y == fruit.cell.y)
						{
							fruit.cell.x = GetRandomValue(0, MAX_GRID_CELLS - 1);
							fruit.cell.y = GetRandomValue(0, MAX_GRID_CELLS - 1);
							i = -1; // Reset the loop to check again
						}
					}
					speedVariable += 0.15f;
	
					// Add a new segment to the snake
					PlayerData newSegment = { 0 };
					newSegment.cell = (Point){ snakeHead.cell.x, snakeHead.cell.y };
					newSegment.color = GREEN;
					newSegment.size = gridCellSize;
					InsertSegment(snakeSegments, newSegment);
				}

				// Move the snake segments in reverse order to avoid overwriting positions (before moving the head)
				for (int i = snakeSegments->size - 1; i > 0; i--)
				{
					PlayerData prevSegment = GetSegment(snakeSegments, i - 1);
					UpdateSegment(snakeSegments, i, prevSegment);
				}

				// Move the head segment based on the last direction
				if (lastDirection == RIGHT || lastDirection == LEFT)
				{
					snakeHead.cell.x += movingDirection;
					frameTimeCount = 0;
				}
				if (lastDirection == UP || lastDirection == DOWN)
				{
					snakeHead.cell.y += movingDirection;
					frameTimeCount = 0;
				}

				// No deaths yet
				if (snakeHead.cell.x < 0) snakeHead.cell.x = 0;
				else if (snakeHead.cell.x >= MAX_GRID_CELLS) snakeHead.cell.x = MAX_GRID_CELLS - 1;
				if (snakeHead.cell.y < 0) snakeHead.cell.y = 0;
				else if (snakeHead.cell.y >= MAX_GRID_CELLS) snakeHead.cell.y = MAX_GRID_CELLS - 1;
	
				// Update the head segment to the new position
				UpdateSegment(snakeSegments, 0, snakeHead);
			}
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
					for (int i = 0; i < snakeSegments->size; i++)
					{
						PlayerData segment = GetSegment(snakeSegments, i);
						DrawRectangle(miniScreenStartX + segment.cell.x*gridCellSize, miniScreenStartY + segment.cell.y*gridCellSize, segment.size, segment.size, GREEN);
					}
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

void InitSnakeArray(snake_array** arr_ptr)
{
	snake_array *container;
	container = (snake_array*)malloc(sizeof(snake_array));
	if (!container) {
		printf("Memory allocation failed for snake_array\n");
		exit(0);
	}

	container->size = 0;
	container->capacity = 4; // initial capacity
	container->array = (PlayerData*)malloc(container->capacity * sizeof(PlayerData));
	if (!container->array) {
		printf("Memory allocation failed for PlayerData array\n");
		exit(0);
	}
	*arr_ptr = container;
}

void FreeSnakeArray(snake_array* container)
{
	free(container->array);
	free(container);
}

void InsertSegment(snake_array* container, PlayerData data)
{
	if (container->size == container->capacity) {
		PlayerData* temp = container->array;
		container->capacity *= 2;
		container->array = (PlayerData*)realloc(container->array, container->capacity * sizeof(PlayerData));
		if (!container->array) {
			printf("Memory reallocation failed for PlayerData array\n");
			container->array = temp;
			return;
		}
	}
	container->array[container->size++] = data;
}

void UpdateSegment(snake_array* container, int index, PlayerData data)
{
	if (index < 0 || index >= container->size) {
		printf("Index out of bounds\n");
		exit(0);
	}
	container->array[index] = data;
}

PlayerData GetSegment(snake_array* container, int index)
{
	if (index < 0 || index >= container->size) {
		printf("Index out of bounds\n");
		exit(0);
	}
	return container->array[index];
}