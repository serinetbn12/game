#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#define SIZE 4
#define TARGET 2048

int grid[SIZE][SIZE];
int total_score = 0;

// Function to initialize the grid
void initializeGrid() {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            grid[r][c] = 0;
        }
    }

    // Add two random values to the grid
    srand(time(NULL));
    for (int i = 0; i < 2; i++) {
        int row, col;
        do {
            row = rand() % SIZE;
            col = rand() % SIZE;
        } while (grid[row][col] != 0);
        grid[row][col] = (rand() % 2 + 1) * 2; // Adds either 2 or 4
    }
}

// Function to print the grid
void displayGrid() {
    printf("\nScore: %d\n", total_score);
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (grid[r][c] == 0)
                printf("[    ] ");
            else
                printf("[%4d] ", grid[r][c]);
        }
        printf("\n");
    }
}

// Function to add a new tile to the grid
void addNewTile() {
    int r, c;
    do {
        r = rand() % SIZE;
        c = rand() % SIZE;
    } while (grid[r][c] != 0);
    grid[r][c] = (rand() % 2 + 1) * 2; // Adds either 2 or 4
}

// Function to handle moving up
void moveUp() {
    for (int c = 0; c < SIZE; c++) {
        int merged[SIZE] = {0}; // Tracks if a tile has merged
        for (int r = 1; r < SIZE; r++) {
            if (grid[r][c] != 0) {
                int row = r;
                while (row > 0 && grid[row - 1][c] == 0) {
                    grid[row - 1][c] = grid[row][c];
                    grid[row][c] = 0;
                    row--;
                }
                if (row > 0 && grid[row - 1][c] == grid[row][c] && !merged[row - 1]) {
                    grid[row - 1][c] *= 2;
                    total_score += grid[row - 1][c];
                    grid[row][c] = 0;
                    merged[row - 1] = 1;
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
                    grid[row + 1][c] = grid[row][c];
                    grid[row][c] = 0;
                    row++;
                }
                if (row < SIZE - 1 && grid[row + 1][c] == grid[row][c] && !merged[row + 1]) {
                    grid[row + 1][c] *= 2;
                    total_score += grid[row + 1][c];
                    grid[row][c] = 0;
                    merged[row + 1] = 1;
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
                    grid[r][col - 1] = grid[r][col];
                    grid[r][col] = 0;
                    col--;
                }
                if (col > 0 && grid[r][col - 1] == grid[r][col] && !merged[col - 1]) {
                    grid[r][col - 1] *= 2;
                    total_score += grid[r][col - 1];
                    grid[r][col] = 0;
                    merged[col - 1] = 1;
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
                    grid[r][col + 1] = grid[r][col];
                    grid[r][col] = 0;
                    col++;
                }
                if (col < SIZE - 1 && grid[r][col + 1] == grid[r][col] && !merged[col + 1]) {
                    grid[r][col + 1] *= 2;
                    total_score += grid[r][col + 1];
                    grid[r][col] = 0;
                    merged[col + 1] = 1;
                }
            }
        }
    }
}

// Function to process user input for game movement
void keymovement(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP:
            moveUp();
            break;
        case SDLK_DOWN:
            moveDown();
            break;
        case SDLK_LEFT:
            moveLeft();
            break;
        case SDLK_RIGHT:
            moveRight();
            break;
        default:
            return;
    }
    addNewTile();
    displayGrid();
}

// Function to check if there are no more valid moves left
int isFull() {
    for (int r = 0; r < SIZE; r++) {
        for (int c = 0; c < SIZE; c++) {
            if (grid[r][c] == 0) return 0;
            if (r > 0 && grid[r][c] == grid[r - 1][c]) return 0;
            if (r < SIZE - 1 && grid[r][c] == grid[r + 1][c]) return 0;
            if (c > 0 && grid[r][c] == grid[r][c - 1]) return 0;
            if (c < SIZE - 1 && grid[r][c] == grid[r][c + 1]) return 0;
        }
    }
    return 1;
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

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "2048 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    initializeGrid();
    displayGrid();

    int isRunning = 1;
    SDL_Event event;

    while (isRunning) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                isRunning = 0;
            } else if (event.type == SDL_KEYDOWN) {
                keymovement(event.key.keysym.sym);
            }
        }

        if (checkWin()) {
            printf("You won!\n");
            break;
        }

        if (isFull()) {
            printf("Game Over!\n");
            break;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

