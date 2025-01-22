#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <string.h>


#define SIZE 4
#define TARGET 2048
#define WINDOW_WIDTH 1000
#define WINDOW_HEIGHT 550
#define GRID_SIZE 100
#define GRID_SPACING 10
#define FONT_SIZE 36

// Structure to store high scores
typedef struct {
    char playerName[50];
    int score;
    int duration;
} HighScore;

// Global variables
HighScore highScores[5] = {0}; // Array to store high scores
int grid[SIZE][SIZE]; // Player's game grid
int machineGrid[SIZE][SIZE]; // Machine's game grid
int total_score = 0; // Player's current score
int machine_score = 0; // Machine's current score
int best_score = 0; // Best score (highest of the 5 stored scores)
Uint32 startTime = 0; // Game start time
int isPaused = 0; // Pause state
int gameMode = 0; // 0: Player, 1: Machine, 2: Player vs Machine
int inMenu = 1; // 1: Show menu, 0: Show game
char playerName[50] = ""; // Player's name
int nameEntered = 0; // Flag to check if name has been entered

// Define menu button rectangles
SDL_Rect buttonPlayer = { (WINDOW_WIDTH - 350) / 2, 150, 350, 60 }; // Player Mode
SDL_Rect buttonMachine = { (WINDOW_WIDTH - 350) / 2, 250, 350, 60 }; // Machine Mode
SDL_Rect buttonPvsM = { (WINDOW_WIDTH - 350) / 2, 350, 350, 60 };    // Player vs Machine
SDL_Rect buttonQuit = { (WINDOW_WIDTH - 350) / 2, 450, 350, 60 };    // Quit



// Function to get the color for a specific tile value
SDL_Color getTileColor(int value) {
    SDL_Color color;
    switch (value) {
        case 2:    color = (SDL_Color){255, 223, 186, 255}; break; // Light Peach
        case 4:    color = (SDL_Color){153, 204, 255, 255}; break; // Light Blue
        case 8:    color = (SDL_Color){255, 143, 143, 255}; break; // Coral
        case 16:   color = (SDL_Color){255, 204, 153, 255}; break; // Light Orange
        case 32:   color = (SDL_Color){255, 153, 102, 255}; break; // Darker Orange
        case 64:   color = (SDL_Color){255, 102, 102, 255}; break; // Red
        case 128:  color = (SDL_Color){204, 255, 153, 255}; break; // Light Green
        case 256:  color = (SDL_Color){153, 255, 153, 255}; break; // Mint Green
        case 512:  color = (SDL_Color){102, 255, 153, 255}; break; // Bright Green
        case 1024: color = (SDL_Color){230, 230, 250, 255}; break; // Lavender
        case 2048: color = (SDL_Color){102, 153, 255, 255}; break; // Royal Blue
        default:   color = (SDL_Color){255, 182, 193, 255}; break; // Default to Light Pink
    }
    return color;
}
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
                    *score += 1; // Increment score for each move
                }
                if (row > 0 && grid[row - 1][c] == grid[row][c] && !merged[row - 1]) {
                    grid[row - 1][c] *= 2; // Merge tiles
                    *score += grid[row - 1][c]; // Update score with merged value
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
        int merged[SIZE] = {0}; // Tracks if a tile has merged
        for (int r = SIZE - 2; r >= 0; r--) {
            if (grid[r][c] != 0) {
                int row = r;
                while (row < SIZE - 1 && grid[row + 1][c] == 0) {
                    grid[row + 1][c] = grid[row][c]; // Move tile down
                    grid[row][c] = 0;
                    row++;
                    *score += 1; // Increment score for each move
                }
                if (row < SIZE - 1 && grid[row + 1][c] == grid[row][c] && !merged[row + 1]) {
                    grid[row + 1][c] *= 2; // Merge tiles
                    *score += grid[row + 1][c]; // Update score with merged value
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
        int merged[SIZE] = {0}; // Tracks if a tile has merged
        for (int c = 1; c < SIZE; c++) {
            if (grid[r][c] != 0) {
                int col = c;
                while (col > 0 && grid[r][col - 1] == 0) {
                    grid[r][col - 1] = grid[r][col]; // Move tile left
                    grid[r][col] = 0;
                    col--;
                    *score += 1; // Increment score for each move
                }
                if (col > 0 && grid[r][col - 1] == grid[r][col] && !merged[col - 1]) {
                    grid[r][col - 1] *= 2; // Merge tiles
                    *score += grid[r][col - 1]; // Update score with merged value
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
        int merged[SIZE] = {0}; // Tracks if a tile has merged
        for (int c = SIZE - 2; c >= 0; c--) {
            if (grid[r][c] != 0) {
                int col = c;
                while (col < SIZE - 1 && grid[r][col + 1] == 0) {
                    grid[r][col + 1] = grid[r][col]; // Move tile right
                    grid[r][col] = 0;
                    col++;
                    *score += 1; // Increment score for each move
                }
                if (col < SIZE - 1 && grid[r][col + 1] == grid[r][col] && !merged[col + 1]) {
                    grid[r][col + 1] *= 2; // Merge tiles
                    *score += grid[r][col + 1]; // Update score with merged value
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
    SDL_Color textColor = {0, 0, 0, 255}; // Black text for contrast
    char text[50];

    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            SDL_Rect cellRect = {c * (GRID_SIZE + GRID_SPACING) + xOffset, r * (GRID_SIZE + GRID_SPACING) + yOffset, GRID_SIZE, GRID_SIZE};

            // Set tile color based on value
            SDL_Color tileColor = getTileColor(grid[r][c]);
            SDL_SetRenderDrawColor(renderer, tileColor.r, tileColor.g, tileColor.b, tileColor.a);
            SDL_RenderFillRect(renderer, &cellRect);

            if (grid[r][c] != 0) {
                sprintf(text, "%d", grid[r][c]);
                SDL_Surface *surface = TTF_RenderText_Solid(font, text, textColor);
                SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

                int textWidth = surface->w;
                int textHeight = surface->h;
                SDL_Rect textRect = {
                    c * (GRID_SIZE + GRID_SPACING) + (GRID_SIZE - textWidth) / 2 + xOffset,
                    r * (GRID_SIZE + GRID_SPACING) + (GRID_SIZE - textHeight) / 2 + yOffset,
                    textWidth,
                    textHeight
                };

                SDL_RenderCopy(renderer, texture, NULL, &textRect);
                SDL_FreeSurface(surface);
                SDL_DestroyTexture(texture);
            }
        }
    }
}

// Function to render the main menu
void renderMainMenu(SDL_Renderer *renderer, TTF_Font *font) {
    // Clear the screen with a black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render the title
    TTF_Font *titleFont = TTF_OpenFont("arial.ttf", 72);
    if (!titleFont) {
        printf("Error loading title font: %s\n", TTF_GetError());
        return;
    }

    char *title = "2048 Game Menu";
    SDL_Color titleColor = {255, 105, 180, 255}; // Pink color
    SDL_Surface *titleSurface = TTF_RenderText_Solid(titleFont, title, titleColor);
    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    int titleX = (WINDOW_WIDTH - titleSurface->w) / 2;
    int titleY = 50;
    SDL_Rect titleRect = {titleX, titleY, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);
    TTF_CloseFont(titleFont);

    // Render menu buttons
    SDL_Color buttonColor = {255, 182, 193, 255}; // Pink color
    SDL_Color textColor = {0, 0, 0, 255}; // Black text

    SDL_Rect buttons[] = {buttonPlayer, buttonMachine, buttonPvsM, buttonQuit};
    char *buttonLabels[] = {"Player Mode", "Machine Mode", "Player vs Machine", "Quit"};

    for (int i = 0; i < 4; i++) {
        SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
        SDL_RenderFillRect(renderer, &buttons[i]);

        SDL_Surface *surface = TTF_RenderText_Solid(font, buttonLabels[i], textColor);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        int textX = buttons[i].x + (buttons[i].w - surface->w) / 2;
        int textY = buttons[i].y + (buttons[i].h - surface->h) / 2;
        SDL_Rect textRect = {textX, textY, surface->w, surface->h};

        SDL_RenderCopy(renderer, texture, NULL, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Update the screen
    SDL_RenderPresent(renderer);
}

// Function to render the pause button
void renderPauseButton(SDL_Renderer *renderer) {
    int buttonRadius = 40; // Radius of the circular button
    int buttonX = WINDOW_WIDTH - 50; // Position in the top-right corner
    int buttonY = 50;

    // Draw the circular button
    SDL_SetRenderDrawColor(renderer, 255, 182, 193, 255); // Pink color
    for (int w = 0; w < buttonRadius * 2; w++) {
        for (int h = 0; h < buttonRadius * 2; h++) {
            int dx = buttonRadius - w; // Horizontal offset
            int dy = buttonRadius - h; // Vertical offset
            if ((dx * dx + dy * dy) <= (buttonRadius * buttonRadius)) {
                SDL_RenderDrawPoint(renderer, buttonX + dx, buttonY + dy);
            }
        }
    }

    // Draw the pause symbol (two vertical bars)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black color

    // Adjust the size and position of the bars
    int barWidth = 10;  // Width of each bar
    int barHeight = 30; // Height of each bar
    int barSpacing = 5; // Space between the bars

    // Calculate the positions of the bars
    SDL_Rect bar1 = {buttonX - barSpacing - barWidth, buttonY - (barHeight / 2), barWidth, barHeight}; // Left bar
    SDL_Rect bar2 = {buttonX + barSpacing, buttonY - (barHeight / 2), barWidth, barHeight}; // Right bar

    // Draw the bars
    SDL_RenderFillRect(renderer, &bar1);
    SDL_RenderFillRect(renderer, &bar2);
}

// Function to render the pause menu
void renderPauseMenu(SDL_Renderer *renderer, TTF_Font *font) {
    // Clear the screen with a semi-transparent overlay
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 200); // Semi-transparent black
    SDL_Rect overlay = {0, 0, WINDOW_WIDTH, WINDOW_HEIGHT};
    SDL_RenderFillRect(renderer, &overlay);

    // Render the title "PAUSE MENU"
    TTF_Font *titleFont = TTF_OpenFont("arial.ttf", 72); // Larger font for the title
    if (!titleFont) {
        printf("Error loading title font: %s\n", TTF_GetError());
        return;
    }

    SDL_Color titleColor = {255, 105, 180, 255}; // Pink color for the title
    SDL_Surface *titleSurface = TTF_RenderText_Solid(titleFont, "PAUSE MENU", titleColor);
    SDL_Texture *titleTexture = SDL_CreateTextureFromSurface(renderer, titleSurface);

    int titleX = (WINDOW_WIDTH - titleSurface->w) / 2; // Center title horizontally
    int titleY = 50; // Position title at the top of the menu
    SDL_Rect titleRect = {titleX, titleY, titleSurface->w, titleSurface->h};
    SDL_RenderCopy(renderer, titleTexture, NULL, &titleRect);

    SDL_FreeSurface(titleSurface);
    SDL_DestroyTexture(titleTexture);
    TTF_CloseFont(titleFont);

    // Define menu buttons
    SDL_Color buttonColor = {255, 182, 193, 255}; // Pink color for buttons
    SDL_Color textColor = {0, 0, 0, 255}; // Black text

    int buttonWidth = 350; // Width of the buttons
    int buttonHeight = 60; // Height of the buttons
    int buttonX = (WINDOW_WIDTH - buttonWidth) / 2; // Center buttons horizontally

    // Resume button
    SDL_Rect resumeButton = {buttonX, 200, buttonWidth, buttonHeight};
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &resumeButton);

    SDL_Surface *resumeSurface = TTF_RenderText_Solid(font, "Resume", textColor);
    SDL_Texture *resumeTexture = SDL_CreateTextureFromSurface(renderer, resumeSurface);
    SDL_Rect resumeTextRect = {
        resumeButton.x + (resumeButton.w - resumeSurface->w) / 2,
        resumeButton.y + (resumeButton.h - resumeSurface->h) / 2,
        resumeSurface->w,
        resumeSurface->h
    };
    SDL_RenderCopy(renderer, resumeTexture, NULL, &resumeTextRect);
    SDL_FreeSurface(resumeSurface);
    SDL_DestroyTexture(resumeTexture);

    // Main Menu button
    SDL_Rect mainMenuButton = {buttonX, 300, buttonWidth, buttonHeight};
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &mainMenuButton);

    SDL_Surface *mainMenuSurface = TTF_RenderText_Solid(font, "Main Menu", textColor);
    SDL_Texture *mainMenuTexture = SDL_CreateTextureFromSurface(renderer, mainMenuSurface);
    SDL_Rect mainMenuTextRect = {
        mainMenuButton.x + (mainMenuButton.w - mainMenuSurface->w) / 2,
        mainMenuButton.y + (mainMenuButton.h - mainMenuSurface->h) / 2,
        mainMenuSurface->w,
        mainMenuSurface->h
    };
    SDL_RenderCopy(renderer, mainMenuTexture, NULL, &mainMenuTextRect);
    SDL_FreeSurface(mainMenuSurface);
    SDL_DestroyTexture(mainMenuTexture);

    // Quit button
    SDL_Rect quitButton = {buttonX, 400, buttonWidth, buttonHeight};
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &quitButton);

    SDL_Surface *quitSurface = TTF_RenderText_Solid(font, "Quit", textColor);
    SDL_Texture *quitTexture = SDL_CreateTextureFromSurface(renderer, quitSurface);
    SDL_Rect quitTextRect = {
        quitButton.x + (quitButton.w - quitSurface->w) / 2,
        quitButton.y + (quitButton.h - quitSurface->h) / 2,
        quitSurface->w,
        quitSurface->h
    };
    SDL_RenderCopy(renderer, quitTexture, NULL, &quitTextRect);
    SDL_FreeSurface(quitSurface);
    SDL_DestroyTexture(quitTexture);

    // Update the screen
    SDL_RenderPresent(renderer);
}

// Function to handle pause menu interactions
int handlePauseMenu(SDL_Event *event) {
    int mouseX = event->button.x;
    int mouseY = event->button.y;

    int buttonWidth = 350;
    int buttonHeight = 60;
    int buttonX = (WINDOW_WIDTH - buttonWidth) / 2;

    SDL_Rect resumeButton = {buttonX, 200, buttonWidth, buttonHeight};
    SDL_Rect mainMenuButton = {buttonX, 300, buttonWidth, buttonHeight};
    SDL_Rect quitButton = {buttonX, 400, buttonWidth, buttonHeight};

    if (mouseX >= resumeButton.x && mouseX <= resumeButton.x + resumeButton.w &&
        mouseY >= resumeButton.y && mouseY <= resumeButton.y + resumeButton.h) {
        return 1; // Resume
    } else if (mouseX >= mainMenuButton.x && mouseX <= mainMenuButton.x + mainMenuButton.w &&
               mouseY >= mainMenuButton.y && mouseY <= mainMenuButton.y + mainMenuButton.h) {
        return 2; // Main Menu
    } else if (mouseX >= quitButton.x && mouseX <= quitButton.x + quitButton.w &&
               mouseY >= quitButton.y && mouseY <= quitButton.y + quitButton.h) {
        return 3; // Quit
    }

    return 0; // No action
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

// Function to save the game state
void saveGameState(int grid[SIZE][SIZE], int score, Uint32 startTime) {
    FILE *file = fopen("saved_game.dat", "wb");
    if (file) {
        fwrite(grid, sizeof(int), SIZE * SIZE, file);
        fwrite(&score, sizeof(int), 1, file);
        fwrite(&startTime, sizeof(Uint32), 1, file);
        fclose(file);
    }
}

// Function to load the game state
void loadGameState(int grid[SIZE][SIZE], int *score, Uint32 *startTime) {
    FILE *file = fopen("saved_game.dat", "rb");
    if (file) {
        fread(grid, sizeof(int), SIZE * SIZE, file);
        fread(score, sizeof(int), 1, file);
        fread(startTime, sizeof(Uint32), 1, file);
        fclose(file);
    }
}

// Function to render directional buttons
void renderDirectionalButtons(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color buttonColor = {255, 182, 193, 255}; // Pink color
    SDL_Rect upButton = {WINDOW_WIDTH - 150, 130, 140, 50};
    SDL_Rect downButton = {WINDOW_WIDTH - 150, 190, 140, 50};
    SDL_Rect leftButton = {WINDOW_WIDTH - 150, 250, 140, 50};
    SDL_Rect rightButton = {WINDOW_WIDTH - 150, 310, 140, 50};

    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &upButton);
    SDL_RenderFillRect(renderer, &downButton);
    SDL_RenderFillRect(renderer, &leftButton);
    SDL_RenderFillRect(renderer, &rightButton);

    SDL_Color textColor = {0, 0, 0, 255}; // Black text
    SDL_Surface *upSurface = TTF_RenderText_Solid(font, "Up", textColor);
    SDL_Surface *downSurface = TTF_RenderText_Solid(font, "Down", textColor);
    SDL_Surface *leftSurface = TTF_RenderText_Solid(font, "Left", textColor);
    SDL_Surface *rightSurface = TTF_RenderText_Solid(font, "Right", textColor);

    SDL_Texture *upTexture = SDL_CreateTextureFromSurface(renderer, upSurface);
    SDL_Texture *downTexture = SDL_CreateTextureFromSurface(renderer, downSurface);
    SDL_Texture *leftTexture = SDL_CreateTextureFromSurface(renderer, leftSurface);
    SDL_Texture *rightTexture = SDL_CreateTextureFromSurface(renderer, rightSurface);

    SDL_Rect upTextRect = {upButton.x + (upButton.w - upSurface->w) / 2, upButton.y + (upButton.h - upSurface->h) / 2, upSurface->w, upSurface->h};
    SDL_Rect downTextRect = {downButton.x + (downButton.w - downSurface->w) / 2, downButton.y + (downButton.h - downSurface->h) / 2, downSurface->w, downSurface->h};
    SDL_Rect leftTextRect = {leftButton.x + (leftButton.w - leftSurface->w) / 2, leftButton.y + (leftButton.h - leftSurface->h) / 2, leftSurface->w, leftSurface->h};
    SDL_Rect rightTextRect = {rightButton.x + (rightButton.w - rightSurface->w) / 2, rightButton.y + (rightButton.h - rightSurface->h) / 2, rightSurface->w, rightSurface->h};

    SDL_RenderCopy(renderer, upTexture, NULL, &upTextRect);
    SDL_RenderCopy(renderer, downTexture, NULL, &downTextRect);
    SDL_RenderCopy(renderer, leftTexture, NULL, &leftTextRect);
    SDL_RenderCopy(renderer, rightTexture, NULL, &rightTextRect);

    SDL_FreeSurface(upSurface);
    SDL_FreeSurface(downSurface);
    SDL_FreeSurface(leftSurface);
    SDL_FreeSurface(rightSurface);
    SDL_DestroyTexture(upTexture);
    SDL_DestroyTexture(downTexture);
    SDL_DestroyTexture(leftTexture);
    SDL_DestroyTexture(rightTexture);
}

// Function to handle machine moves
void machineMove(int grid[SIZE][SIZE], int *score) {
    int move = rand() % 4; // Randomly choose a move (0: UP, 1: DOWN, 2: LEFT, 3: RIGHT)
    switch (move) {
        case 0:
            moveUp(grid, score); // Update the machine's score
            break;
        case 1:
            moveDown(grid, score); // Update the machine's score
            break;
        case 2:
            moveLeft(grid, score); // Update the machine's score
            break;
        case 3:
            moveRight(grid, score); // Update the machine's score
            break;
    }
    addNewTile(grid); // Add a new tile after the move
}

// Function to render game info (score, best score, and time)
void renderGameInfo(SDL_Renderer *renderer, TTF_Font *font) {
    SDL_Color color = {255, 255, 255, 255}; // White text
    char infoText[100];

    if (gameMode == 1) { // Machine Mode
        sprintf(infoText, "Machine Score: %d | Best: %d | Time: %d sec",
                machine_score, best_score, (SDL_GetTicks() - startTime) / 1000);
    } else if (gameMode == 2) { // Player vs Machine Mode
        sprintf(infoText, "Player Score: %d | Machine Score: %d | Best: %d | Time: %d sec",
                total_score, machine_score, best_score, (SDL_GetTicks() - startTime) / 1000);
    } else { // Player Mode
        sprintf(infoText, "Score: %d | Best: %d | Time: %d sec",
                total_score, best_score, (SDL_GetTicks() - startTime) / 1000);
    }

    // Use a smaller font for the score and time display
    TTF_Font *smallFont = TTF_OpenFont("arial.ttf", 33); // Smaller font size (24)
    if (!smallFont) {
        printf("Error loading small font: %s\n", TTF_GetError());
        return;
    }

    SDL_Surface *surface = TTF_RenderText_Solid(smallFont, infoText, color); // Render text
    SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface); // Create texture
    SDL_Rect textRect = {10, 10, surface->w, surface->h}; // Position at top-left
    SDL_RenderCopy(renderer, texture, NULL, &textRect); // Draw texture

    // Clean up
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    TTF_CloseFont(smallFont);
}

// Function to save high scores
void saveScores(HighScore highScores[], int count) {
    FILE *file = fopen("highscores.dat", "wb");
    if (file) {
        // Save only the top 5 scores
        fwrite(highScores, sizeof(HighScore), (count > 5) ? 5 : count, file);
        fclose(file);
    }
}

// Function to load high scores
void loadScores(HighScore highScores[], int *count) {
    FILE *file = fopen("highscores.dat", "rb");
    if (file) {
        // Load up to 5 scores
        *count = fread(highScores, sizeof(HighScore), 5, file);
        fclose(file);

        // Sort the loaded scores
        sortScores(highScores, *count);

        // Update best_score to the highest score in the array
        best_score = highScores[0].score;
    } else {
        *count = 0; // No scores loaded
    }
}

// Function to sort high scores
void sortScores(HighScore highScores[], int count) {
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (highScores[i].score < highScores[j].score) {
                HighScore temp = highScores[i];
                highScores[i] = highScores[j];
                highScores[j] = temp;
            }
        }
    }
}

// Function to add a new high score
void addHighScore(HighScore highScores[], int *count, HighScore newScore) {
    // If there are fewer than 5 scores, add the new score
    if (*count < 5) {
        highScores[*count] = newScore;
        (*count)++;
    } else {
        // If there are already 5 scores, replace the lowest score if the new score is higher
        if (newScore.score > highScores[4].score) {
            highScores[4] = newScore;
        }
    }

    // Sort the scores in descending order
    sortScores(highScores, *count);

    // Update best_score to the highest score in the array
    best_score = highScores[0].score;
}

// Function to render high scores
void renderHighScores(SDL_Renderer *renderer, TTF_Font *font, HighScore highScores[], int count) {
    // Constants for table layout
    const int COL_WIDTH = 200; // Width of each column
    const int ROW_HEIGHT = 50; // Height of each row
    const int PADDING = 10; // Padding inside cells
    const int BORDER_WIDTH = 2; // Width of table borders

    // Colors
    SDL_Color backgroundColor = {30, 30, 30, 255}; // Dark gray background
    SDL_Color headerColor = {255,182,193,255}; // oink for headers
    SDL_Color textColor = {255, 255, 255, 255}; // White for text
    SDL_Color borderColor = {100, 100, 100, 255}; // Gray for borders

    // Clear the screen with a black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Calculate table dimensions
    int tableWidth = 4 * COL_WIDTH; // Total width of the table
    int tableHeight = (count + 1) * ROW_HEIGHT; // Total height of the table
    int startX = (WINDOW_WIDTH - tableWidth) / 2; // Centered horizontally
    int startY = (WINDOW_HEIGHT - tableHeight) / 2; // Centered vertically

    // Draw the table background
    SDL_Rect tableRect = {startX, startY, tableWidth, tableHeight};
    SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
    SDL_RenderFillRect(renderer, &tableRect);

    // Draw table borders
    SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, borderColor.a);
    for (int i = 0; i <= 4; i++) {
        // Vertical borders
        SDL_RenderDrawLine(renderer, startX + i * COL_WIDTH, startY, startX + i * COL_WIDTH, startY + tableHeight);
    }
    for (int i = 0; i <= count + 1; i++) {
        // Horizontal borders
        SDL_RenderDrawLine(renderer, startX, startY + i * ROW_HEIGHT, startX + tableWidth, startY + i * ROW_HEIGHT);
    }

    // Draw table headers
    char *headers[] = {"Rank", "Name", "Score", "Time (sec)"};
    for (int i = 0; i < 4; i++) {
        SDL_Surface *surface = TTF_RenderText_Solid(font, headers[i], headerColor);
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

        // Center text in the cell
        int textX = startX + i * COL_WIDTH + (COL_WIDTH - surface->w) / 2;
        int textY = startY + PADDING;

        SDL_Rect rect = {textX, textY, surface->w, surface->h};
        SDL_RenderCopy(renderer, texture, NULL, &rect);

        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }

    // Draw high scores data
    for (int i = 0; i < count && i < 5; i++) {
        char rankText[10], scoreText[20], timeText[20];
        sprintf(rankText, "%d", i + 1); // Rank
        sprintf(scoreText, "%d", highScores[i].score); // Score
        sprintf(timeText, "%d", highScores[i].duration); // Time

        // Draw rank
        SDL_Surface *rankSurface = TTF_RenderText_Solid(font, rankText, textColor);
        SDL_Texture *rankTexture = SDL_CreateTextureFromSurface(renderer, rankSurface);
        int rankX = startX + (COL_WIDTH - rankSurface->w) / 2;
        int rankY = startY + (i + 1) * ROW_HEIGHT + PADDING;
        SDL_Rect rankRect = {rankX, rankY, rankSurface->w, rankSurface->h};
        SDL_RenderCopy(renderer, rankTexture, NULL, &rankRect);
        SDL_FreeSurface(rankSurface);
        SDL_DestroyTexture(rankTexture);

        // Draw name
        SDL_Surface *nameSurface = TTF_RenderText_Solid(font, highScores[i].playerName, textColor);
        SDL_Texture *nameTexture = SDL_CreateTextureFromSurface(renderer, nameSurface);
        int nameX = startX + COL_WIDTH + (COL_WIDTH - nameSurface->w) / 2;
        int nameY = startY + (i + 1) * ROW_HEIGHT + PADDING;
        SDL_Rect nameRect = {nameX, nameY, nameSurface->w, nameSurface->h};
        SDL_RenderCopy(renderer, nameTexture, NULL, &nameRect);
        SDL_FreeSurface(nameSurface);
        SDL_DestroyTexture(nameTexture);

        // Draw score
        SDL_Surface *scoreSurface = TTF_RenderText_Solid(font, scoreText, textColor);
        SDL_Texture *scoreTexture = SDL_CreateTextureFromSurface(renderer, scoreSurface);
        int scoreX = startX + 2 * COL_WIDTH + (COL_WIDTH - scoreSurface->w) / 2;
        int scoreY = startY + (i + 1) * ROW_HEIGHT + PADDING;
        SDL_Rect scoreRect = {scoreX, scoreY, scoreSurface->w, scoreSurface->h};
        SDL_RenderCopy(renderer, scoreTexture, NULL, &scoreRect);
        SDL_FreeSurface(scoreSurface);
        SDL_DestroyTexture(scoreTexture);

        // Draw time
        SDL_Surface *timeSurface = TTF_RenderText_Solid(font, timeText, textColor);
        SDL_Texture *timeTexture = SDL_CreateTextureFromSurface(renderer, timeSurface);
        int timeX = startX + 3 * COL_WIDTH + (COL_WIDTH - timeSurface->w) / 2;
        int timeY = startY + (i + 1) * ROW_HEIGHT + PADDING;
        SDL_Rect timeRect = {timeX, timeY, timeSurface->w, timeSurface->h};
        SDL_RenderCopy(renderer, timeTexture, NULL, &timeRect);
        SDL_FreeSurface(timeSurface);
        SDL_DestroyTexture(timeTexture);
    }

    // Update the screen
    SDL_RenderPresent(renderer);

    // Wait for user input to exit
    int waiting = 1;
    SDL_Event event;
    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_KEYDOWN || event.type == SDL_QUIT) {
                waiting = 0; // Exit the loop
            }
        }
        SDL_Delay(16); // Reduce CPU usage
    }
}

// Function to render the "Game Over" screen
void renderGameOver(SDL_Renderer *renderer, TTF_Font *font) {
    // Clear the screen with a black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render "Game Over" text
    SDL_Color textColor = {255, 0, 0, 255}; // Red text
    char *gameOverText = "Game Over!";
    SDL_Surface *gameOverSurface = TTF_RenderText_Solid(font, gameOverText, textColor);
    SDL_Texture *gameOverTexture = SDL_CreateTextureFromSurface(renderer, gameOverSurface);
    SDL_Rect gameOverRect = {
        (WINDOW_WIDTH - gameOverSurface->w) / 2, // Center horizontally
        100, // Position vertically
        gameOverSurface->w,
        gameOverSurface->h
    };
    SDL_RenderCopy(renderer, gameOverTexture, NULL, &gameOverRect);
    SDL_FreeSurface(gameOverSurface);
    SDL_DestroyTexture(gameOverTexture);

    // Render "Go to Main Menu" button
    SDL_Color buttonColor = {255, 182, 193, 255}; // Pink color
    SDL_Rect menuButton = {
        (WINDOW_WIDTH - 200) / 2, // Center horizontally
        200, // Position below the "Game Over" text
        200, // Button width
        50   // Button height
    };
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &menuButton);

    // Render button text
    char *buttonText = "Main Menu";
    SDL_Surface *buttonSurface = TTF_RenderText_Solid(font, buttonText, textColor);
    SDL_Texture *buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_Rect buttonTextRect = {
        menuButton.x + (menuButton.w - buttonSurface->w) / 2, // Center text horizontally
        menuButton.y + (menuButton.h - buttonSurface->h) / 2, // Center text vertically
        buttonSurface->w,
        buttonSurface->h
    };
    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonTextRect);
    SDL_FreeSurface(buttonSurface);
    SDL_DestroyTexture(buttonTexture);

    // Update the screen
    SDL_RenderPresent(renderer);

    // Wait for user input
    int waiting = 1;
    SDL_Event event;
    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                waiting = 0; // Exit the game
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                // Check if the "Main Menu" button is clicked
                if (mouseX >= menuButton.x && mouseX <= menuButton.x + menuButton.w &&
                    mouseY >= menuButton.y && mouseY <= menuButton.y + menuButton.h) {
                    waiting = 0; // Exit the loop and return to the main menu
                }
            }
        }
        SDL_Delay(16); // Reduce CPU usage
    }
}

// Function to render the "You Win" screen
void renderWin(SDL_Renderer *renderer, TTF_Font *font) {
    // Clear the screen with a black background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    // Render "You Win" text
    SDL_Color textColor = {0, 255, 0, 255}; // Green text
    char *winText = "You Win!";
    SDL_Surface *winSurface = TTF_RenderText_Solid(font, winText, textColor);
    SDL_Texture *winTexture = SDL_CreateTextureFromSurface(renderer, winSurface);
    SDL_Rect winRect = {
        (WINDOW_WIDTH - winSurface->w) / 2, // Center horizontally
        100, // Position vertically
        winSurface->w,
        winSurface->h
    };
    SDL_RenderCopy(renderer, winTexture, NULL, &winRect);
    SDL_FreeSurface(winSurface);
    SDL_DestroyTexture(winTexture);

    // Render "Go to Main Menu" button
    SDL_Color buttonColor = {255, 182, 193, 255}; // Pink color
    SDL_Rect menuButton = {
        (WINDOW_WIDTH - 200) / 2, // Center horizontally
        200, // Position below the "You Win" text
        200, // Button width
        50   // Button height
    };
    SDL_SetRenderDrawColor(renderer, buttonColor.r, buttonColor.g, buttonColor.b, buttonColor.a);
    SDL_RenderFillRect(renderer, &menuButton);

    // Render button text
    char *buttonText = "Main Menu";
    SDL_Surface *buttonSurface = TTF_RenderText_Solid(font, buttonText, textColor);
    SDL_Texture *buttonTexture = SDL_CreateTextureFromSurface(renderer, buttonSurface);
    SDL_Rect buttonTextRect = {
        menuButton.x + (menuButton.w - buttonSurface->w) / 2, // Center text horizontally
        menuButton.y + (menuButton.h - buttonSurface->h) / 2, // Center text vertically
        buttonSurface->w,
        buttonSurface->h
    };
    SDL_RenderCopy(renderer, buttonTexture, NULL, &buttonTextRect);
    SDL_FreeSurface(buttonSurface);
    SDL_DestroyTexture(buttonTexture);

    // Update the screen
    SDL_RenderPresent(renderer);

    // Wait for user input
    int waiting = 1;
    SDL_Event event;
    while (waiting) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                waiting = 0; // Exit the game
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                // Check if the "Main Menu" button is clicked
                if (mouseX >= menuButton.x && mouseX <= menuButton.x + menuButton.w &&
                    mouseY >= menuButton.y && mouseY <= menuButton.y + menuButton.h) {
                    waiting = 0; // Exit the loop and return to the main menu
                }
            }
        }
        SDL_Delay(16); // Reduce CPU usage
    }
}
void resetGameState() {
    // Reset player grid and score
    initializeGrid(grid);
    total_score = 0;

    // Reset machine grid and score (if applicable)
    if (gameMode == 1 || gameMode == 2) {
        initializeGrid(machineGrid);
        machine_score = 0;
    }

    // Reset the timer
    startTime = SDL_GetTicks();
}
// Main function
int main(int argc, char* argv[]) {
    // Initialize SDL and SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("SDL Error: %s\n", SDL_GetError());
        return -1;
    }

    // Load high scores
    int highScoreCount = 0;
    loadScores(highScores, &highScoreCount);

    // Create a window and renderer
    SDL_Window *window = SDL_CreateWindow("2048 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    TTF_Font *font = TTF_OpenFont("arial.ttf", FONT_SIZE); // Load font with larger size

    if (!window || !renderer || !font) {
        printf("Error: %s\n", SDL_GetError());
        SDL_Quit();
        return -1;
    }

    // Initialize grids
    initializeGrid(grid);
    initializeGrid(machineGrid);

    // Game state variables
    int isRunning = 1; // Main loop flag
    Uint32 machineMoveTime = SDL_GetTicks(); // Timer for machine moves
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

    // Main game loop
    while (isRunning) {
        // Event handling
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                // Save the current score before quitting
                if (total_score > 0) {
                    HighScore newScore;
                    strncpy(newScore.playerName, playerName, sizeof(newScore.playerName) - 1);
                    newScore.score = total_score; // Save only the player's score
                    newScore.duration = (SDL_GetTicks() - startTime) / 1000;

                    addHighScore(highScores, &highScoreCount, newScore);
                    saveScores(highScores, highScoreCount);
                }
                isRunning = 0; // Exit the game
            } else if (event.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX = event.button.x;
                int mouseY = event.button.y;

                if (inMenu) {
                    // Check if a menu button is clicked
                    if (mouseX >= buttonPlayer.x && mouseX <= buttonPlayer.x + buttonPlayer.w &&
                        mouseY >= buttonPlayer.y && mouseY <= buttonPlayer.y + buttonPlayer.h) {
                        gameMode = 0; // Player Mode
                        resetGameState(); // Reset the game state
                        startTime = SDL_GetTicks();
                        inMenu = 0; // Exit the menu
                    } else if (mouseX >= buttonMachine.x && mouseX <= buttonMachine.x + buttonMachine.w &&
                               mouseY >= buttonMachine.y && mouseY <= buttonMachine.y + buttonMachine.h) {
                        gameMode = 1; // Machine Mode
                        resetGameState(); // Reset the game state
                        startTime = SDL_GetTicks();
                        inMenu = 0; // Exit the menu
                    } else if (mouseX >= buttonPvsM.x && mouseX <= buttonPvsM.x + buttonPvsM.w &&
                               mouseY >= buttonPvsM.y && mouseY <= buttonPvsM.y + buttonPvsM.h) {
                        gameMode = 2; // Player vs Machine Mode
                        resetGameState(); // Reset the game state
                        startTime = SDL_GetTicks();
                        inMenu = 0; // Exit the menu
                    } else if (mouseX >= buttonQuit.x && mouseX <= buttonQuit.x + buttonQuit.w &&
                               mouseY >= buttonQuit.y && mouseY <= buttonQuit.y + buttonQuit.h) {
                        isRunning = 0; // Quit the game
                    }
                } else {
                    // Handle pause button click
                    int buttonRadius = 25;
                    int buttonX = WINDOW_WIDTH - 50;
                    int buttonY = 50;
                    int dx = mouseX - buttonX;
                    int dy = mouseY - buttonY;
                    if (dx * dx + dy * dy <= buttonRadius * buttonRadius) {
                        isPaused = !isPaused; // Toggle pause state
                    }

                    // Handle pause menu interactions
                    if (isPaused) {
                        int action = handlePauseMenu(&event);
                        if (action == 1) {
                            isPaused = 0; // Resume
                        } else if (action == 2) {
                            // Save the current score before returning to the main menu
                            if (total_score > 0) {
                                HighScore newScore;
                                strncpy(newScore.playerName, playerName, sizeof(newScore.playerName) - 1);
                                newScore.score = total_score; // Save only the player's score
                                newScore.duration = (SDL_GetTicks() - startTime) / 1000;

                                addHighScore(highScores, &highScoreCount, newScore);
                                saveScores(highScores, highScoreCount);
                            }
                            isPaused = 0;
                            inMenu = 1; // Return to main menu
                        } else if (action == 3) {
                            // Save the current score before quitting
                            if (total_score > 0) {
                                HighScore newScore;
                                strncpy(newScore.playerName, playerName, sizeof(newScore.playerName) - 1);
                                newScore.score = total_score; // Save only the player's score
                                newScore.duration = (SDL_GetTicks() - startTime) / 1000;

                                addHighScore(highScores, &highScoreCount, newScore);
                                saveScores(highScores, highScoreCount);
                            }
                            isRunning = 0; // Quit
                        }
                    }
                }
            } else if (event.type == SDL_KEYDOWN) {
                // Handle keyboard input for movement
                if (!inMenu && !isPaused && (gameMode == 0 || gameMode == 2)) {
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

        // Machine move logic (only in Machine or Player vs Machine mode)
        if ((gameMode == 1 || gameMode == 2) && !isPaused) {
            if (SDL_GetTicks() - machineMoveTime > 1000) { // Machine moves every 1 second
                machineMove(machineGrid, &machine_score); // Pass machine_score by reference
                machineMoveTime = SDL_GetTicks(); // Reset the timer
            }
        }

        // Clear the screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
        SDL_RenderClear(renderer);

        if (inMenu) {
            renderMainMenu(renderer, font); // Render the main menu
        } else {
            // Render game info (score, best score, and time)
            renderGameInfo(renderer, font);

            // Render pause button
            renderPauseButton(renderer);

            // Center the grids horizontally
            int playerGridX = (WINDOW_WIDTH / 2 - (SIZE * (GRID_SIZE + GRID_SPACING))) / 2;
            int machineGridX = WINDOW_WIDTH / 2 + (WINDOW_WIDTH / 2 - (SIZE * (GRID_SIZE + GRID_SPACING))) / 2;
            int gridY = 100; // Move the grid down

            // Render player's grid (if in Player or Player vs Machine mode)
            if (gameMode == 0 || gameMode == 2) {
                renderGrid(renderer, font, grid, playerGridX, gridY, &total_score);
            }

            // Render machine's grid (if in Machine or Player vs Machine mode)
            if (gameMode == 1 || gameMode == 2) {
                renderGrid(renderer, font, machineGrid, machineGridX, gridY, &machine_score);
            }

            // Render pause menu if paused
            if (isPaused) {
                renderPauseMenu(renderer, font);
            }
        }

        // Update the screen
        SDL_RenderPresent(renderer);

        // Check for win condition
        if (checkWin(grid)) {
            renderWin(renderer, font);
            inMenu = 1; // Return to menu
        }

        // Check for game over condition
        if (isFull(grid)) {
            renderGameOver(renderer, font);
            inMenu = 1; // Return to menu
        }

        SDL_Delay(16); // Add a small delay to reduce CPU usage
    }

    // Render high scores before quitting
    renderHighScores(renderer, font, highScores, highScoreCount);

    // Clean up resources
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
