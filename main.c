#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h> // For _kbhit() and _getch()

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define TETROMINO_SIZE 4
#define FALL_SPEED 4
#define MAX_TETROMINOES 60

// Representing the game board
int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};

typedef enum {
    I, O, T, S, Z, J, L, NUM_TETROMINOS
} TetrominoType;

int tetrominos[NUM_TETROMINOS][TETROMINO_SIZE][TETROMINO_SIZE] = {
    // I
    {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {0, 0, 0, 0},
        {0, 0, 0, 0}
    },
    // O
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    },
    // T
    {
        {0, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 1, 0, 0},
        {0, 0, 0, 0}
    },
    // S
    {
        {0, 0, 0, 0},
        {0, 1, 1, 0},
        {1, 1, 0, 0},
        {0, 0, 0, 0}
    },
    // Z
    {
        {0, 0, 0, 0},
        {1, 1, 0, 0},
        {0, 1, 1, 0},
        {0, 0, 0, 0}
    },
    // J
    {
        {0, 0, 0, 0},
        {1, 0, 0, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}
    },
    // L
    {
        {0, 0, 0, 0},
        {0, 0, 1, 0},
        {1, 1, 1, 0},
        {0, 0, 0, 0}
    }
};


typedef struct {
    int x, y; // Position on the board
    int shape[TETROMINO_SIZE][TETROMINO_SIZE];
} Tetromino;

Tetromino currentTetromino;

void fixTetromino() {
    // Update the board to reflect the Tetromino's final position
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (currentTetromino.shape[i][j] == 1) {
                int boardX = currentTetromino.x + j;
                int boardY = currentTetromino.y + i;
                if (boardX >= 0 && boardX < BOARD_WIDTH && boardY < BOARD_HEIGHT) {
                    board[boardY][boardX] = 1;
                }
            }
        }
    }
}

void createTetromino(TetrominoType type) {
    currentTetromino.x = BOARD_WIDTH / 2 - TETROMINO_SIZE / 2;
    currentTetromino.y = 0;
    memcpy(currentTetromino.shape, tetrominos[type], sizeof(currentTetromino.shape));
}

bool canMove(int newX, int newY) {
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (currentTetromino.shape[i][j] == 1) {
                int boardX = newX + j;
                int boardY = newY + i;

                // Check against the floor and walls
                if (boardY >= BOARD_HEIGHT || boardX < 0 || boardX >= BOARD_WIDTH) {
                    return false;
                }

                // Check against other fixed blocks
                if (boardY >= 0 && board[boardY][boardX] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

void moveTetromino(int deltaX, int deltaY) {
    int newX = currentTetromino.x + deltaX;
    int newY = currentTetromino.y + deltaY;
    if (canMove(newX, newY)) {
        currentTetromino.x = newX;
        currentTetromino.y = newY;
    }
}

void rotateTetromino(int shape[TETROMINO_SIZE][TETROMINO_SIZE], int newShape[TETROMINO_SIZE][TETROMINO_SIZE]) {
    // Transpose the matrix
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            newShape[j][i] = shape[i][j];
        }
    }

    // Reverse the order of the columns
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE / 2; j++) {
            int temp = newShape[i][j];
            newShape[i][j] = newShape[i][TETROMINO_SIZE - 1 - j];
            newShape[i][TETROMINO_SIZE - 1 - j] = temp;
        }
    }
}

bool canRotate(int newShape[TETROMINO_SIZE][TETROMINO_SIZE], int x, int y) {
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            if (newShape[i][j] == 1) {
                int boardX = x + j;
                int boardY = y + i;

                // Check against the floor and walls
                if (boardY >= BOARD_HEIGHT || boardX < 0 || boardX >= BOARD_WIDTH) {
                    return false;
                }

                // Check against other fixed blocks
                if (boardY >= 0 && board[boardY][boardX] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

// Function to initialize the board
void initBoard() {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board[y][x] = 0;
        }
    }
}

// Function to print the board
void printBoard() {
    system("cls"); // Clear the console

    // Top border (optional)
    for (int x = 0; x < BOARD_WIDTH + 2; x++) {
        printf("# ");
    }
    printf("\n");

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        printf("# "); // Left border
        for (int x = 0; x < BOARD_WIDTH; x++) {
            // Check if the current cell is part of the active Tetromino
            bool isPartOfTetromino = false;
            for (int i = 0; i < TETROMINO_SIZE; i++) {
                for (int j = 0; j < TETROMINO_SIZE; j++) {
                    if (currentTetromino.shape[i][j] == 1) {
                        if (y == currentTetromino.y + i && x == currentTetromino.x + j) {
                            printf("A ");
                            isPartOfTetromino = true;
                            break;
                        }
                    }
                }
                if (isPartOfTetromino) break;
            }

            if (!isPartOfTetromino) {
                printf("%c ", board[y][x] ? '#' : ' ');
            }
        }
        printf("#\n"); // Right border
    }

    // Bottom border
    for (int x = 0; x < BOARD_WIDTH + 2; x++) {
        printf("# ");
    }
    printf("\n");
}

int main() {
    initBoard();
    int tetrominoCount = 0;
    //createTetromino(T); // Create an initial Tetromino, e.g., T-shaped
    createTetromino(rand() % NUM_TETROMINOS); // Create a random Tetromino
    tetrominoCount++;
    int counter = 0;

    while (tetrominoCount <= MAX_TETROMINOES) {
         if (_kbhit()) {
        char key = _getch();
        switch (key) {
            case 'a': // Move left
            case 'A':
                if (canMove(currentTetromino.x - 1, currentTetromino.y)) {
                    moveTetromino(-1, 0);
                }
                break;
            case 'd': // Move right
            case 'D':
                if (canMove(currentTetromino.x + 1, currentTetromino.y)) {
                    moveTetromino(1, 0);
                }
                break;
            case 's': // Move down
            case 'S':
                if (canMove(currentTetromino.x, currentTetromino.y + 1)) {
                    moveTetromino(0, 1);
                }
                break;
            case 'w': // Rotate
            case 'W':
                int newShape[TETROMINO_SIZE][TETROMINO_SIZE];
                rotateTetromino(currentTetromino.shape, newShape);
                if (canRotate(newShape, currentTetromino.x, currentTetromino.y)) {
                memcpy(currentTetromino.shape, newShape, sizeof(newShape));
                }
                break;
            // Add other controls as needed
        }
    }

        // Automatically move the Tetromino down or fix it if it can't move down
        if (counter >= FALL_SPEED) {
        if (canMove(currentTetromino.x, currentTetromino.y + 1)) {
            moveTetromino(0, 1); // Move down
        }
        else {
            fixTetromino(); // Fix the Tetromino in place
            createTetromino(rand() % NUM_TETROMINOS); // Create a new Tetromino
            tetrominoCount++;
        }
        counter = 0; // Reset the counter
        }
        else {
        counter++; // Increment the counter
        }

        printBoard(); // Print the updated board

        _sleep(100); // Sleep for a delay (adjust as needed)

        // Game over or max Tetromino count reached
        printf("Game Over\n");
    }

    return 0;
}
