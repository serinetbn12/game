#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

#define SIZE 4
#define TARGET 2048
#define WINDOW_WIDTH 1000  // Wider window to fit two grids
#define WINDOW_HEIGHT 550  // Reasonable height
#define GRID_SIZE 100      // Slightly larger grid cells
#define GRID_SPACING 10    // Spacing between cells
#define FONT_SIZE 36       // Slightly larger font size

typedef struct {
    char playerName[50];
    int score;
    int duration;
} HighScore;

HighScore highScores[5] = {0}; // Array to store high scores
int grid[SIZE][SIZE]; // Player's game grid
int machineGrid[SIZE][SIZE]; // Machine's game grid
int total_score = 0; // Player's current score
int machine_score = 0; // Machine's current score
int best_score = 0; // Best score
Uint32 startTime = 0; // Game start time

int gameMode = 0; // 0: Player, 1: Machine, 2: Player vs Machine
int inMenu = 1; // 1: Show menu, 0: Show game
char playerName[50] = ""; // Player's name
int nameEntered = 0; // Flag to check if name has been entered

// Function to initialize the grid
void initializeGrid(int grid[SIZE][SIZE]) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            grid[r][c] = 0; // Set all cells to 0
        }
    }

    // Add two random values to the grid
    srand(time(NULL));
    for (int i = 0; i < 2; i++) {
        int row, col;
        do {
            row = rand() % SIZE;
            col = rand() % SIZE;
        } while (grid[row][col] != 0); // Find an empty cell
        grid[row][col] = (rand() % 2 + 1) * 2; // Add either 2 or 4
    }
}

// Function to add a new tile to the grid
void addNewTile(int grid[SIZE][SIZE]) {
    int r, c;
    do {
        r = rand() % SIZE;
        c = rand() % SIZE;
    } while (grid[r][c] != 0); // Find an empty cell
    grid[r][c] = (rand() % 2 + 1) * 2; // Add either 2 or 4
}

// Function to handle moving up
void moveUp(int grid[SIZE][SIZE], int *score) {
    for (int c = 0; c < SIZE; c++) {
        int merged[SIZE] = {0}; // Tracks if a tile has merged
        for (int r = 1; r < SIZE; r++) {
            if (grid[r][c] != 0) {
                int row = r;
                while (row > 0 && grid[row - 1][c] == 0) {
                    grid[row - 1][c] = grid[row][c]; // Move tile up
                    grid[row][c] = 0;
                    row--;
                }
                if (row > 0 && grid[row - 1][c] == grid[row][c] && !merged[row - 1]) {
                    grid[row - 1][c] *= 2; // Merge tiles
                    *score += grid[row - 1][c]; // Update score
                    grid[row][c] = 0;
                    merged[row - 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Function to handle moving down
void moveDown(int grid[SIZE][SIZE], int *score) {
    for (int c = 0; c < SIZE; c++) {
        int merged[SIZE] = {0};
        for (int r = SIZE - 2; r >= 0; r--) {
            if (grid[r][c] != 0) {
                int row = r;
                while (row < SIZE - 1 && grid[row + 1][c] == 0) {
                    grid[row + 1][c] = grid[row][c]; // Move tile down
                    grid[row][c] = 0;
                    row++;
                }
                if (row < SIZE - 1 && grid[row + 1][c] == grid[row][c] && !merged[row + 1]) {
                    grid[row + 1][c] *= 2; // Merge tiles
                    *score += grid[row + 1][c]; // Update score
                    grid[row][c] = 0;
                    merged[row + 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Function to handle moving left
void moveLeft(int grid[SIZE][SIZE], int *score) {
    for (int r = 0; r < SIZE; r++) {
        int merged[SIZE] = {0};
        for (int c = 1; c < SIZE; c++) {
            if (grid[r][c] != 0) {
                int col = c;
                while (col > 0 && grid[r][col - 1] == 0) {
                    grid[r][col - 1] = grid[r][col]; // Move tile left
                    grid[r][col] = 0;
                    col--;
                }
                if (col > 0 && grid[r][col - 1] == grid[r][col] && !merged[col - 1]) {
                    grid[r][col - 1] *= 2; // Merge tiles
                    *score += grid[r][col - 1]; // Update score
                    grid[r][col] = 0;
                    merged[col - 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Function to handle moving right
void moveRight(int grid[SIZE][SIZE], int *score) {
    for (int r = 0; r < SIZE; r++) {
        int merged[SIZE] = {0};
        for (int c = SIZE - 2; c >= 0; c--) {
            if (grid[r][c] != 0) {
                int col = c;
                while (col < SIZE - 1 && grid[r][col + 1] == 0) {
                    grid[r][col + 1] = grid[r][col]; // Move tile right
                    grid[r][col] = 0;
                    col++;
                }
                if (col < SIZE - 1 && grid[r][col + 1] == grid[r][col] && !merged[col + 1]) {
                    grid[r][col + 1] *= 2; // Merge tiles
                    *score += grid[r][col + 1]; // Update score
                    grid[r][col] = 0;
                    merged[col + 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Function to check if there are no more valid moves left
int isFull(int grid[SIZE][SIZE]) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (grid[r][c] == 0) return 0; // Empty cell found
            if (r > 0 && grid[r][c] == grid[r - 1][c]) return 0; // Merge possible
            if (r < SIZE - 1 && grid[r][c] == grid[r + 1][c]) return 0; // Merge possible
            if (c > 0 && grid[r][c] == grid[r][c - 1]) return 0; // Merge possible
            if (c < SIZE - 1 && grid[r][c] == grid[r][c + 1]) return 0; // Merge possible
        }
    }
    return 1; // No valid moves left
}

// Function to check if the player has won
int checkWin(int grid[SIZE][SIZE]) {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (grid[r][c] == TARGET) {
                return 1; // Player has won
            }
        }
    }
    return 0; // Player has not won
}

// Function to render the grid
void renderGrid(SDL_Renderer *renderer, TTF_Font *font, int grid[SIZE][SIZE], int xOffset, int yOffset, int *score) {
    SDL_Color color = {255, 255, 255, 255}; // Set text color to white
    char text[50];

    // Render the grid
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            SDL_Rect cellRect = {c * (GRID_SIZE + GRID_SPACING) + xOffset, r * (GRID_SIZE + GRID_SPACING) + yOffset, GRID_SIZE, GRID_SIZE};

            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &cellRect);

            if (grid[r][c] != 0) {
                sprintf(text, "%d", grid[r][c]); // Convert cell value to string
                SDL_Surface *surface = TTF_RenderText_Solid(font, text, color); // Render text
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Create texture

                int textWidth = surface->w;
                int textHeight = surface->h;
                SDL_Rect textRect = {
                    c * (GRID_SIZE + GRID_SPACING) + (GRID_SIZE - textWidth) / 2 + xOffset,
                    r * (GRID_SIZE + GRID_SPACING) + (GRID_SIZE - textHeight) / 2 + yOffset,
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, texture, NULL, &textRect); // Draw texture
                SDL_FreeSurface(surface); // Free surface
                SDL_DestroyTexture(texture); // Free texture
            }
        }
    }
}

// Function to render the main menu
void renderMainMenu(SDL_Renderer *renderer, TTF_Font *font) {
    // Clear the screen with a black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(renderer);

    // Render the title
    char *title = "2048 Game Menu";
    SDL_Color titleColor = {255, 215, 0, 255}; // Gold color for the title
    SDL_Surface *titleSurface = TTF_RenderText_Solid(font, title, titleColor);
    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    // Center the title horizontally
    int titleX = (WINDOW_WIDTH - titleSurface->w) / 2;
    int titleY = 50; // Vertical position of the title
    SDL_Rect titleRect = {titleX, titleY, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    // Free the title surface and texture
    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);

    // Button dimensions and positions
    int buttonWidth = 300; // Wider buttons
    int buttonHeight = 60; // Taller buttons
    int buttonX = (WINDOW_WIDTH - buttonWidth) / 2; // Center buttons horizontally
    int buttonY = 150; // Vertical position of the first button
    int buttonSpacing = 100; // More space between buttons

    // Define button rectangles
    SDL_Rect buttonPlayer = {buttonX, buttonY, buttonWidth, buttonHeight};          // Player Mode
    SDL_Rect buttonMachine = {buttonX, buttonY + buttonSpacing, buttonWidth, buttonHeight};   // Machine Mode
    SDL_Rect buttonPvsM = {buttonX, buttonY + 2 * buttonSpacing, buttonWidth, buttonHeight};  // Player vs Machine
    SDL_Rect buttonQuit = {buttonX, buttonY + 3 * buttonSpacing, buttonWidth, buttonHeight};  // Quit

    // Draw buttons with a green color
    SDL_Color buttonColor = {50, 205, 50, 255}; // Light green (RGB: 50, 205, 50)
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &buttonPlayer);
    SDL_RenderFillRect(renderer, &buttonMachine);
    SDL_RenderFillRect(renderer, &buttonPvsM);
    SDL_RenderFillRect(renderer, &buttonQuit);

    // Render text on buttons
    SDL_Color textColor = {255, 255, 255, 255}; // White text
    char *menuItems[] = {"Player Mode", "Machine Mode", "Player vs Machine", "Quit"};
    SDL_Rect buttonRects[] = {buttonPlayer, buttonMachine, buttonPvsM, buttonQuit};

    for (int i = 0; i < 4; i++) {
        SDL_Surface *surface = TTF_RenderText_Solid(font, menuItems[i], textColor); // Create text surface
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Create texture from surface

        // Center text within the button
        SDL_Rect textRect = {
            buttonRects[i].x + (buttonRects[i].w - surface->w) / 2,
            buttonRects[i].y + (buttonRects[i].h - surface->h) / 2,
            surface->w,
            surface->h
        };

        SDL_RenderCopy(renderer, texture, NULL, &textRect); // Render text
        SDL_FreeSurface(surface); // Free surface
        SDL_DestroyTexture(texture); // Free texture
    }

    // Update the screen
    SDL_RenderPresent(renderer);
}

// Function to render the name input screen
void renderNameInput(SDL_Renderer *renderer, TTF_Font *font, const char *inputText) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background color to black
    SDL_RenderClear(renderer); // Clear the screen

    SDL_Color color = {255, 255, 255, 255}; // Set text color to white
    char promptText[] = "Enter your name: ";
    char fullText[100];
    sprintf(fullText, "%s%s", promptText, inputText);

    SDL_Surface *surface = TTF_RenderText_Solid(font, fullText, color); // Render text
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Create texture
    SDL_Rect textRect = {10, 10, surface->w, surface->h}; // Set position and size
    SDL_RenderCopy(renderer, texture, NULL, &textRect); // Draw texture
    SDL_FreeSurface(surface); // Free surface
    SDL_DestroyTexture(texture); // Free texture

    SDL_RenderPresent(renderer); // Update the screen
}

// Function to handle machine moves
void machineMove(int grid[SIZE][SIZE], int *score) {
    int move = rand() % 4; // Randomly choose a move (0: UP, 1: DOWN, 2: LEFT, 3: RIGHT)
    switch (move) {
        case 0:
            moveUp(grid, score);
            break;
        case 1:
            moveDown(grid, score);
            break;
        case 2:
            moveLeft(grid, score);
            break;
        case 3:
            moveRight(grid, score);
            break;
    }
    addNewTile(grid); // Add a new tile after the move
}

// Function to render game info (score, best score, and time)
void renderGameInfo(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color color = {255, 255, 255, 255}; // White text
    char infoText[100];
    sprintf(infoText, "Player Score: %d | Machine Score: %d | Best: %d | Time: %d sec", 
            total_score, machine_score, best_score, (SDL_GetTicks() - startTime) / 1000);

    SDL_Surface *surface = TTF_RenderText_Solid(font, infoText, color); // Render text
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Create texture
    SDL_Rect textRect = {10, 10, surface->w, surface->h}; // Position at top-left
    SDL_RenderCopy(renderer, texture, NULL, &textRect); // Draw texture
    SDL_FreeSurface(surface); // Free surface
    SDL_DestroyTexture(texture); // Free texture
}

int main(int argc, char* argv[]) {
    // Initialize SDL and SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create a window and renderer
    SDL_Window *window = SDL_CreateWindow("2048 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("arial.ttf", FONT_SIZE); // Load font with larger size

    if (!window || !renderer || !font) {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    int isRunning = 1; // Main loop flag
    SDL_Event event;

    // Name input loop
    char inputText[50] = "";
    int nameEntered = 0;
    SDL_StartTextInput(); // Enable text input

    while (!nameEntered) {
        renderNameInput(renderer, font, inputText); // Render the name input screen

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = 0;
                nameEntered = 1; // Exit the game
            } else if (event.type == SDL_KEYDOWN) {
                if (event.key.keysym.sym == SDLK_RETURN) {
                    strncpy(playerName, inputText, sizeof(playerName) - 1);
                    nameEntered = 1; // Exit the name input loop
                } else if (event.key.keysym.sym == SDLK_BACKSPACE && strlen(inputText) > 0) {
                    inputText[strlen(inputText) - 1] = '\0'; // Remove last character
                }
            } else if (event.type == SDL_TEXTINPUT) {
                if (strlen(inputText) < sizeof(inputText) - 1) {
                    strcat(inputText, event.text.text); // Append the entered character
                }
            }
        }
        SDL_Delay(16); // Add a small delay to reduce CPU usage
    }

    SDL_StopTextInput(); // Disable text input

    // Initialize grids
    initializeGrid(grid);
    initializeGrid(machineGrid);

    Uint32 machineMoveTime = SDL_GetTicks(); // Timer for machine moves

    while (isRunning) {
        if (inMenu) {
            renderMainMenu(renderer, font); // Render the main menu
        } else {
            // Clear the screen
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
            SDL_RenderClear(renderer);

            // Render game info (score, best score, and time)
            renderGameInfo(renderer, font);

            // Center the grids horizontally
            int playerGridX = (WINDOW_WIDTH / 2 - (SIZE * (GRID_SIZE + GRID_SPACING))) / 2;
            int machineGridX = WINDOW_WIDTH / 2 + (WINDOW_WIDTH / 2 - (SIZE * (GRID_SIZE + GRID_SPACING))) / 2;

            // Render player's grid (if in Player or Player vs Machine mode)
            if (gameMode == 0 || gameMode == 2) {
                renderGrid(renderer, font, grid, playerGridX, 100, &total_score);
            }

            // Render machine's grid (if in Machine or Player vs Machine mode)
            if (gameMode == 1 || gameMode == 2) {
                renderGrid(renderer, font, machineGrid, machineGridX, 100, &machine_score);
            }

            // Update the screen
            SDL_RenderPresent(renderer);
        }

        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = 0; // Exit the game
            } else if (event.type == SDL_KEYDOWN) {
                if (inMenu) {
                    // Handle menu input
                    switch (event.key.keysym.sym) {
                        case SDLK_1:
                            gameMode = 0; // Player mode
                            initializeGrid(grid);
                            startTime = SDL_GetTicks();
                            inMenu = 0;
                            break;
                        case SDLK_2:
                            gameMode = 1; // Machine mode
                            initializeGrid(machineGrid);
                            startTime = SDL_GetTicks();
                            inMenu = 0;
                            break;
                        case SDLK_3:
                            gameMode = 2; // Player vs Machine mode
                            initializeGrid(grid);
                            initializeGrid(machineGrid);
                            startTime = SDL_GetTicks();
                            inMenu = 0;
                            break;
                        case SDLK_q:
                            isRunning = 0; // Quit the game
                            break;
                        default:
                            break;
                    }
                } else {
                    // Handle game input (only in Player or Player vs Machine mode)
                    if (gameMode == 0 || gameMode == 2) {
                        switch (event.key.keysym.sym) {
                            case SDLK_UP:
                                moveUp(grid, &total_score);
                                addNewTile(grid);
                                break;
                            case SDLK_DOWN:
                                moveDown(grid, &total_score);
                                addNewTile(grid);
                                break;
                            case SDLK_LEFT:
                                moveLeft(grid, &total_score);
                                addNewTile(grid);
                                break;
                            case SDLK_RIGHT:
                                moveRight(grid, &total_score);
                                addNewTile(grid);
                                break;
                            default:
                                break;
                        }
                    }
                }
            }
        }

        // Machine move logic (only in Machine or Player vs Machine mode)
        if (gameMode == 1 || gameMode == 2) {
            if (SDL_GetTicks() - machineMoveTime > 1000) { // Machine moves every 1 second
                machineMove(machineGrid, &machine_score);
                machineMoveTime = SDL_GetTicks();
            }
        }

        SDL_Delay(16); // Add a small delay to reduce CPU usage
    }

    // Clean up resources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
