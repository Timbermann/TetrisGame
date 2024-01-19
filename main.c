#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h> // For _kbhit() and _getch()

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define TETROMINO_SIZE 4
#define FALL_SPEED 10

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

void createTetromino(TetrominoType type) {
    currentTetromino.x = BOARD_WIDTH / 2 - TETROMINO_SIZE / 2;
    currentTetromino.y = 0;
    memcpy(currentTetromino.shape, tetrominos[type], sizeof(currentTetromino.shape));
}

bool canMove(int newX, int newY) {
    // Collision detection logic here
    // Return true if the Tetromino can move to the new position
}

void moveTetromino(int deltaX, int deltaY) {
    int newX = currentTetromino.x + deltaX;
    int newY = currentTetromino.y + deltaY;
    if (canMove(newX, newY)) {
        currentTetromino.x = newX;
        currentTetromino.y = newY;
    }
}

void rotateTetromino() {
    int temp[TETROMINO_SIZE][TETROMINO_SIZE];

    // Transpose the matrix
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE; j++) {
            temp[j][i] = currentTetromino.shape[i][j];
        }
    }

    // Reverse the order of the columns
    for (int i = 0; i < TETROMINO_SIZE; i++) {
        for (int j = 0; j < TETROMINO_SIZE / 2; j++) {
            int tmp = temp[i][j];
            temp[i][j] = temp[i][TETROMINO_SIZE - j - 1];
            temp[i][TETROMINO_SIZE - j - 1] = tmp;
        }
    }

    // Check if rotation is possible
    // if (canRotate(temp)) {
        memcpy(currentTetromino.shape, temp, sizeof(temp));
    // }
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

    // Draw top border
    for (int x = 0; x < BOARD_WIDTH + 2; x++) {
        printf("# ");
    }
    printf("\n");

    for (int y = 0; y < BOARD_HEIGHT; y++) {
        printf("# "); // Left border
        for (int x = 0; x < BOARD_WIDTH; x++) {
            // Check if the current position is part of the Tetromino
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

    // Draw bottom border
    for (int x = 0; x < BOARD_WIDTH + 2; x++) {
        printf("# ");
    }
    printf("\n");
}



int main() {
    initBoard();
    //createTetromino(T); // Create an initial Tetromino, e.g., T-shaped
    createTetromino(rand() % NUM_TETROMINOS); // Create a random Tetromino
    int counter = 0;

    while (true) {
        if (_kbhit()) {
            char key = _getch();
            switch (key) {
                case 'a': // Move left
                case 'A':
                    moveTetromino(-1, 0);
                    break;
                case 'd': // Move right
                case 'D':
                    moveTetromino(1, 0);
                    break;
                case 's': // Move down
                case 'S':
                    moveTetromino(0, 1);
                    break;
                case 'w': // Rotate
                case 'W':
                    rotateTetromino();
                    break;
                // Add other controls as needed
            }
        }

        // Move the Tetromino down at regular intervals
        if (counter >= FALL_SPEED) {
            moveTetromino(0, 1); // Move down
            counter = 0; // Reset the counter
        } else {
            counter++; // Increment the counter
        }

        printBoard(); // Print the updated board

        _sleep(100); // Sleep for a delay (adjust as needed)
    }

    return 0;
}
