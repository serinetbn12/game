
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>

#define SIZE 4
#define WIN_SCORE 2048

int board[SIZE][SIZE];
int score = 0;
//inistialisation
void initializeBoard() {
    // Initialize the board with zeros
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            board[i][j] = 0;
        }
    }

    // Add two initial random numbers
    srand(time(NULL));
    for (int k = 0; k < 2; k++) {
        int x, y;
        do {
            x = rand() % SIZE;
            y = rand() % SIZE;
        } while (board[x][y] != 0);
        board[x][y] = (rand() % 2 + 1) * 2; // 2 or 4
    }
}

void printBoard() {
    printf("\nScore: %d\n", score);
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0)
                printf("[    ] ");
            else
                printf("[%4d] ", board[i][j]);
        }
        printf("\n");
    }
}

void spawnNewNumber() {
    int x, y;
    do {
        x = rand() % SIZE;
        y = rand() % SIZE;
    } while (board[x][y] != 0);
    board[x][y] = (rand() % 2 + 1) * 2; // 2 or 4
}

void moveUp() {
    for (int j = 0; j < SIZE; j++) {
        int merge[SIZE] = {0}; // Track merged cells
        for (int i = 1; i < SIZE; i++) {
            if (board[i][j] != 0) {
                int k = i;
                while (k > 0 && board[k - 1][j] == 0) {
                    board[k - 1][j] = board[k][j];
                    board[k][j] = 0;
                    k--;
                }
                if (k > 0 && board[k - 1][j] == board[k][j] && !merge[k - 1]) {
                    board[k - 1][j] *= 2;
                    score += board[k - 1][j];
                    board[k][j] = 0;
                    merge[k - 1] = 1;
                }
            }
        }
    }
}

void moveDown() {
    for (int j = 0; j < SIZE; j++) {
        int merge[SIZE] = {0};
        for (int i = SIZE - 2; i >= 0; i--) {
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

void handleKey(SDL_Keycode key) {
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
    spawnNewNumber();
    printBoard();
}
int is_full() {
    // Check if the board is full
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                return 0; // Board is not full
            }
        }
    }
    return 1; // Board is full
}

int is_won() {
    // Check if the player has won
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == WIN_SCORE) {
                return 1; // Player has won
            }
        }
    }
    return 0; // Player has not won
}

int can_move() {
    // Check if there are any valid moves left
    for (int i = 0; i < SIZE; i++) {
        for (int j = 0; j < SIZE; j++) {
            if (board[i][j] == 0) {
                return 1; // There are still empty cells to move
            }
            if (j > 0 && board[i][j] == board[i][j - 1]) {
                return 1; // Can move left
            }
            if (j < SIZE - 1 && board[i][j] == board[i][j + 1]) {
                return 1; // Can move right
            }
            if (i > 0 && board[i][j] == board[i - 1][j]) {
                return 1; // Can move up
            }
            if (i < SIZE - 1 && board[i][j] == board[i + 1][j]) {
                return 1; // Can move down
            }
        }
    }
    return 0; // No valid moves left
}
int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_VIDEO);

    SDL_Window* window = SDL_CreateWindow(
        "2048 Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    initializeBoard();
    printBoard();

    int running = 1;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            } else if (event.type == SDL_KEYDOWN) {
                handleKey(event.key.keysym.sym);
            }
        }
    }
 while (1) {
        if (is_won()) {
            printf("You won!\n");
            break;
        }

        if (is_full() && !can_move()) {
            printf("Game over!\n");
            break;
        }


    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
