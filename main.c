
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#define SIZE 4 // The board is 4x4
#define WIN_SCORE 2048 // This is how you win the game

int board[SIZE][SIZE]; // The game board
int score = 0; // Player's score

// Set up the board (empty to start, then add two random numbers)
void initializeBoard() {
    // Fill the board with zeros (empty spaces)
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 0;
        }
    }

    // Seed random numbers
    srand(time(NULL));

    // Add two starting tiles (2 or 4)
    for (int k = 0; k < 2; k++) {
        int x, y;
        do {
            x = rand() % SIZE; // Random row
            y = rand() % SIZE; // Random column
        } while (board[x][y] != 0); // Make sure the spot is empty
        board[x][y] = (rand() % 2 + 1) * 2; // Either 2 or 4
    }
}

// Print the board in the console
void printBoard() {
    printf("\nScore: %d\n", score);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0)
                printf("[    ] "); // Empty cell
            else
                printf("[%4d] ", board[i][j]); // Tile with value
        }
        printf("\n");
    }
}

// Add a new random number to the board (after a move)
void spawnNewNumber() {
    int x, y;
    do {
        x = rand() % SIZE; // Random row
        y = rand() % SIZE; // Random column
    } while (board[x][y] != 0); // Keep trying if spot is taken
    board[x][y] = (rand() % 2 + 1) * 2; // Add either 2 or 4
}

// Move tiles up and combine if possible
void moveUp() {
    for (int j = 0; j < SIZE; j++) { // Loop through columns
        int merge[SIZE] = {0}; // Keep track of merges
        for (int i = 1; i < SIZE; i++) { // Start from the second row
            if (board[i][j] != 0) { // If the tile isn't empty
                int k = i;
                while (k > 0 && board[k - 1][j] == 0) { // Move up if empty
                    board[k - 1][j] = board[k][j];
                    board[k][j] = 0;
                    k--;
                }
                if (k > 0 && board[k - 1][j] == board[k][j] && !merge[k - 1]) { // Merge if same value
                    board[k - 1][j] *= 2;
                    score += board[k - 1][j]; // Add to score
                    board[k][j] = 0;
                    merge[k - 1] = 1; // Mark as merged
                }
            }
        }
    }
}

// Move tiles down
void moveDown() {
    for (int j = 0; j < SIZE; j++) {
        int merge[SIZE] = {0};
        for (int i = SIZE - 2; i >= 0; i--) { // Start from second last row
            if (board[i][j] != 0) {
                int k = i;
                while (k < SIZE - 1 && board[k + 1][j] == 0) {
                    board[k + 1][j] = board[k][j];
                    board[k][j] = 0;
                    k++;
                }
                if (k < SIZE - 1 && board[k + 1][j] == board[k][j] && !merge[k + 1]) {
                    board[k + 1][j] *= 2;
                    score += board[k + 1][j];
                    board[k][j] = 0;
                    merge[k + 1] = 1;
                }
            }
        }
    }
}

// Move tiles left
void moveLeft() {
    for (int i = 0; i < SIZE; i++) {
        int merge[SIZE] = {0};
        for (int j = 1; j < SIZE; j++) {
            if (board[i][j] != 0) {
                int k = j;
                while (k > 0 && board[i][k - 1] == 0) {
                    board[i][k - 1] = board[i][k];
                    board[i][k] = 0;
                    k--;
                }
                if (k > 0 && board[i][k - 1] == board[i][k] && !merge[k - 1]) {
                    board[i][k - 1] *= 2;
                    score += board[i][k - 1];
                    board[i][k] = 0;
                    merge[k - 1] = 1;
                }
            }
        }
    }
}

// Move tiles right
void moveRight() {
    for (int i = 0; i < SIZE; i++) {
        int merge[SIZE] = {0};
        for (int j = SIZE - 2; j >= 0; j--) {
            if (board[i][j] != 0) {
                int k = j;
                while (k < SIZE - 1 && board[i][k + 1] == 0) {
                    board[i][k + 1] = board[i][k];
                    board[i][k] = 0;
                    k++;
                }
                if (k < SIZE - 1 && board[i][k + 1] == board[i][k] && !merge[k + 1]) {
                    board[i][k + 1] *= 2;
                    score += board[i][k + 1];
                    board[i][k] = 0;
                    merge[k + 1] = 1;
                }
            }
        }
    }
}

// Handle key presses for movement
void handleKey(SDL_Keycode key) {
    switch (key) {
        case SDLK_UP: moveUp(); break;
        case SDLK_DOWN: moveDown(); break;
        case SDLK_LEFT: moveLeft(); break;
        case SDLK_RIGHT: moveRight(); break;
        default: return;
    }
    spawnNewNumber(); // Add a new number after each valid move
    printBoard(); // Show the updated board
}

// Check if the player won or lost
int is_won() {
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == WIN_SCORE) return 1; // 2048 reached
        }
    }
    return 0;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow("2048 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    initializeBoard();
    printBoard();

    SDL_Event event;
    int running = 1;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) running = 0;
            else if (event.type == SDL_KEYDOWN) handleKey(event.key.keysym.sym);
        }

        if (is_won()) {
            printf("Congrats, you won!\n");
            break;
        }
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

