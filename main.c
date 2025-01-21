#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>

#define SIZE 4
#define TARGET 2048

typedef struct {
    char playerName[50];
    int score;
    int duration;
} HighScore;

HighScore highScores[5] = {0}; // Array to store high scores
int grid[SIZE][SIZE]; // Game grid
int total_score = 0; // Current score
int best_score = 0; // Best score
Uint32 startTime = 0; // Game start time

int gameMode = 0; // 0: Player, 1: Machine, 2: Player vs Machine
int inMenu = 1; // 1: Show menu, 0: Show game
char playerName[50] = ""; // Player's name
int nameEntered = 0; // Flag to check if name has been entered

// Function to initialize the grid
void initializeGrid() {
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
void addNewTile() {
    int r, c;
    do {
        r = rand() % SIZE;
        c = rand() % SIZE;
    } while (grid[r][c] != 0); // Find an empty cell
    grid[r][c] = (rand() % 2 + 1) * 2; // Add either 2 or 4
}

// Function to handle moving up
void moveUp() {
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
                    total_score += grid[row - 1][c]; // Update score
                    grid[row][c] = 0;
                    merged[row - 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Function to handle moving down
void moveDown() {
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
                    total_score += grid[row + 1][c]; // Update score
                    grid[row][c] = 0;
                    merged[row + 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Function to handle moving left
void moveLeft() {
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
                    total_score += grid[r][col - 1]; // Update score
                    grid[r][col] = 0;
                    merged[col - 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Function to handle moving right
void moveRight() {
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
                    total_score += grid[r][col + 1]; // Update score
                    grid[r][col] = 0;
                    merged[col + 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Function to check if there are no more valid moves left
int isFull() {
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
int checkWin() {
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
void renderGrid(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background color to black
    SDL_RenderClear(renderer); // Clear the screen

    SDL_Color color = {255, 255, 255, 255}; // Set text color to white
    char text[50];

    // Display current score, best score, and elapsed time
    sprintf(text, "Score: %d", total_score);
    SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
    SDL_Rect scoreRect = {10, 10, scoreSurface->w, scoreSurface->h};
    SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
    SDL_FreeSurface(scoreSurface);
    SDL_DestroyTexture(scoreTexture);

    sprintf(text, "Best Score: %d", best_score);
    SDL_Surface *bestScoreSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *bestScoreTexture = SDL_CreateTextureFromSurface(renderer, bestScoreSurface);
    SDL_Rect bestScoreRect = {10, 40, bestScoreSurface->w, bestScoreSurface->h};
    SDL_RenderCopy(renderer, bestScoreTexture, NULL, &bestScoreRect);
    SDL_FreeSurface(bestScoreSurface);
    SDL_DestroyTexture(bestScoreTexture);

    Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;
    sprintf(text, "Time: %d sec", elapsedTime);
    SDL_Surface *timeSurface = TTF_RenderText_Solid(font, text, color);
    SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
    SDL_Rect timeRect = {10, 70, timeSurface->w, timeSurface->h};
    SDL_RenderCopy(renderer, timeTexture, NULL, &timeRect);
    SDL_FreeSurface(timeSurface);
    SDL_DestroyTexture(timeTexture);

    // Render the grid
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            SDL_Rect cellRect = {c * 110 + 10, r * 110 + 100, 100, 100};

            SDL_SetRenderDrawColor(renderer, 50, 50, 50, 255);
            SDL_RenderFillRect(renderer, &cellRect);

            if (grid[r][c] != 0) {
                sprintf(text, "%d", grid[r][c]); // Convert cell value to string
                SDL_Surface *surface = TTF_RenderText_Solid(font, text, color); // Render text
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Create texture

                int textWidth = surface->w;
                int textHeight = surface->h;
                SDL_Rect textRect = {
                    c * 110 + (110 - textWidth) / 2 + 10,
                    r * 110 + (110 - textHeight) / 2 + 100,
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, texture, NULL, &textRect); // Draw texture
                SDL_FreeSurface(surface); // Free surface
                SDL_DestroyTexture(texture); // Free texture
            }
        }
    }
    SDL_RenderPresent(renderer); // Update the screen
}

// Function to load high scores from a file
void loadHighScores() {
    FILE *file = fopen("highscores.dat", "rb");
    if (file != NULL) {
        fread(highScores, sizeof(HighScore), 5, file); // Read high scores
        fclose(file);
    } else {
        // Initialize default scores if file not found
        for (int i = 0; i < 5; i++) {
            sprintf(highScores[i].playerName, "Player%d", i + 1);
            highScores[i].score = (5 - i) * 10;
            highScores[i].duration = (5 - i) * 60;
        }
    }
}

// Function to save high scores to a file
void saveHighScores() {
    FILE *file = fopen("highscores.dat", "wb");
    if (file != NULL) {
        fwrite(highScores, sizeof(HighScore), 5, file); // Write high scores
        fclose(file);
    }
}


// Function to render high scores
void renderHighScores(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color color = {255, 255, 255, 255}; // Set text color to white
    char text[100];

    for (int i = 0; i < 5; ++i) {
        if (highScores[i].score > 0) {
            sprintf(text, "%d. %s - %d pts - %d sec", i + 1, highScores[i].playerName, highScores[i].score, highScores[i].duration);
            SDL_Surface *surface = TTF_RenderText_Solid(font, text, color); // Render text
            SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Create texture
            SDL_Rect textRect = {10, 50 + i * 30, surface->w, surface->h}; // Set position and size
            SDL_RenderCopy(renderer, texture, NULL, &textRect); // Draw texture
            SDL_FreeSurface(surface); // Free surface
            SDL_DestroyTexture(texture); // Free texture
        }
    }
}

// Function to render the main menu
void renderMainMenu(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Set background color to black
    SDL_RenderClear(renderer); // Clear the screen

    SDL_Color color = {255, 255, 255, 255}; // Set text color to white
    char *menuItems[] = {"1. Player Mode", "2. Machine Mode", "3. Player vs Machine Mode", "Press Q to Quit"};
    for (int i = 0; i < 4; i++) {
        SDL_Surface *surface = TTF_RenderText_Solid(font, menuItems[i], color); // Render text
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Create texture
        SDL_Rect textRect = {10, 50 + i * 30, surface->w, surface->h}; // Set position and size
        SDL_RenderCopy(renderer, texture, NULL, &textRect); // Draw texture
        SDL_FreeSurface(surface); // Free surface
        SDL_DestroyTexture(texture); // Free texture
    }
    SDL_RenderPresent(renderer); // Update the screen
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

int main(int argc, char* argv[]) {
    // Initialize SDL and SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    // Create a window and renderer
    SDL_Window *window = SDL_CreateWindow("2048 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 600, 550, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("arial.ttf", 24); // Load font

    if (!window || !renderer || !font) {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    loadHighScores(); // Load high scores

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

    while (isRunning) {
        if (inMenu) {
            renderMainMenu(renderer, font); // Render the main menu
        } else {
            renderGrid(renderer, font); // Render the game grid
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
                            initializeGrid(); // Initialize the grid
                            startTime = SDL_GetTicks(); // Start the timer
                            inMenu = 0; // Exit menu
                            break;
                        case SDLK_2:
                            gameMode = 1; // Machine mode
                            initializeGrid(); // Initialize the grid
                            startTime = SDL_GetTicks(); // Start the timer
                            inMenu = 0; // Exit menu
                            break;
                        case SDLK_3:
                            gameMode = 2; // Player vs Machine mode
                            initializeGrid(); // Initialize the grid
                            startTime = SDL_GetTicks(); // Start the timer
                            inMenu = 0; // Exit menu
                            break;
                        case SDLK_q:
                            isRunning = 0; // Quit the game
                            break;
                        default:
                            break;
                    }
                } else {
   switch (event.key.keysym.sym){
                        case SDLK_UP:
                            moveUp();
                            addNewTile();
                            break;
                        case SDLK_DOWN:
                            moveDown();
                            addNewTile();
                            break;
                        case SDLK_LEFT:
                            moveLeft();
                            addNewTile();
                            break;
                        case SDLK_RIGHT:
                            moveRight();
                            addNewTile();
                            break;
                        default:
                            break;
                    }
                }
            }
        }

        if (!inMenu ) {
            if (checkWin()) {
                printf("You won!\n");
                isRunning = 0; // Exit the game
            }

            if (isFull()) {
                printf("Game Over!\n");
                isRunning = 0; // Exit the game
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
