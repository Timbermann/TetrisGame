#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h> // For _kbhit() and _getch()
#include <string.h>

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20
#define TETROMINO_SIZE 4
#define MAX_TETROMINOES 60
#define MAX_SCORES 10
#define MAX_TOTAL_SCORES 1000

int displayMenu() {
    int choice;
    do {
        system("cls");  // Clear the screen
        printf("TETRIS\n");
        printf("1. Start New Game\n");
        printf("2. Options\n");
        printf("3. Leaderboard\n");
        printf("4. End\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
    } while (choice < 1 || choice > 4);  // Validate input

    return choice;
}

// Representing the game board
int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};

typedef enum {
    I, O, T, S, Z, J, L, NUM_TETROMINOS
} TetrominoType;

TetrominoType bag[NUM_TETROMINOS];
int nextIndex = 0;
int score = 0;
int fallSpeed = 5;

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

void fillAndShuffleBag() {
    // Fill the bag with one of each type
    for (int i = 0; i < NUM_TETROMINOS; i++) {
        bag[i] = (TetrominoType)i;
    }

    // Shuffle the bag
    for (int i = 0; i < NUM_TETROMINOS; i++) {
        int j = rand() % NUM_TETROMINOS;
        TetrominoType temp = bag[i];
        bag[i] = bag[j];
        bag[j] = temp;
    }

    nextIndex = 0;
}

TetrominoType getNextTetromino() {
    if (nextIndex >= NUM_TETROMINOS) {
        fillAndShuffleBag();
    }
    return bag[nextIndex++];
}

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

                // Check against the floor
                if (boardY >= BOARD_HEIGHT) return false;

                // Check against the left and right borders
                if (boardX < 0 || boardX >= BOARD_WIDTH) return false;

                // Check against other fixed blocks (only if within board bounds)
                if (boardY < BOARD_HEIGHT && board[boardY][boardX] == 1) {
                    return false;
                }
            }
        }
    }
    return true;
}

void dropTetromino() {
    while (canMove(currentTetromino.x, currentTetromino.y + 1)) {
        currentTetromino.y++;
    }
    fixTetromino(); // Fix the Tetromino in place after dropping
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

void clearFullLines() {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        bool lineIsFull = true;
        for (int x = 0; x < BOARD_WIDTH; x++) {
            if (board[y][x] == 0) {
                lineIsFull = false;
                break;
            }
        }

        if (lineIsFull) {
            // Shift everything above this line down by one
            for (int yy = y; yy > 0; yy--) {
                for (int x = 0; x < BOARD_WIDTH; x++) {
                    board[yy][x] = board[yy - 1][x];
                }
            }

            // Clear the top line
            for (int x = 0; x < BOARD_WIDTH; x++) {
                board[0][x] = 0;
            }

            // Since we just shifted a line down, we need to check this line again
            y--;
            score++;
        }
    }
}


// Function to initialize the board
void initBoard() {
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            board[y][x] = 0;
        }
    }
}

void writeScoreToFile(int finalScore) {
    FILE *file = fopen("scores.txt", "a"); // Open the file in append mode
    if (file != NULL) {
        fprintf(file, "Score: %d\n", finalScore); // Write the score on a new line
        fclose(file); // Close the file
    } else {
        printf("Unable to save the score.\n");
    }
}

void readAndSortScores(int bestScores[], int *numScores) {
    FILE *file = fopen("scores.txt", "r");
    if (file == NULL) {
        printf("No scores available.\n");
        *numScores = 0;
        return;
    }

    // Read all scores into an array
    int allScores[MAX_TOTAL_SCORES];
    int score, totalScores = 0;
    while (fscanf(file, "Score: %d\n", &score) == 1 && totalScores < MAX_TOTAL_SCORES) {
        allScores[totalScores++] = score;
    }
    fclose(file);

    // Sort all scores in descending order
    for (int i = 0; i < totalScores - 1; i++) {
        for (int j = i + 1; j < totalScores; j++) {
            if (allScores[i] < allScores[j]) {
                int temp = allScores[i];
                allScores[i] = allScores[j];
                allScores[j] = temp;
            }
        }
    }

    // Select the top 10 scores (or fewer if there aren't enough)
    *numScores = totalScores < MAX_SCORES ? totalScores : MAX_SCORES;
    for (int i = 0; i < *numScores; i++) {
        bestScores[i] = allScores[i];
    }
}

void displayLeaderboard() {
    int scores[MAX_SCORES], numScores;
    readAndSortScores(scores, &numScores);

    printf("Leaderboard:\n");
    for (int i = 0; i < numScores; i++) {
        printf("%d. %d\n", i + 1, scores[i]);
    }

    if (numScores == 0) {
        printf("No scores available.\n");
    }

    printf("\nPress 'X' to get back to menu.\n");
}

void displayOptionsMenu() {
    int newSpeed;
    printf("Current Fall Speed: %d\n", fallSpeed);
    printf("Enter new fall speed (1-10, where 1 is the fastest and 10 is the slowest): ");
    scanf("%d", &newSpeed);

    if (newSpeed >= 1 && newSpeed <= 10) {
        fallSpeed = newSpeed;
    } else {
        printf("Invalid speed. Keeping current speed.\n");
    }

    _sleep(2000); // Wait for 2 seconds before returning to the menu
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
    printf("\n");
    printf("Score: %d\n", score);
}

int main() {
    srand(time(NULL)); // Seed the random number generator
    int tetrominoCount = 0;
    int counter = 0;
    score = 0;
    char key;
    int menuChoice;

    while (true) {  // Main loop
        menuChoice = displayMenu();

        switch (menuChoice) {
            case 1:

                printf("Starting New Game...\n");
                _sleep(1000);
                fillAndShuffleBag(); // Initially fill and shuffle the bag
                initBoard();
                tetrominoCount = 0;
                //createTetromino(T); // Create an initial Tetromino, e.g., T-shaped
                createTetromino(rand() % NUM_TETROMINOS); // Create a random Tetromino
                tetrominoCount++;
                counter = 0;
                score = 0;

                while (tetrominoCount <= MAX_TETROMINOES) {
                    if (_kbhit()) {
                    key = _getch();
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
                         case ' ': // Drop Tetromino
                            dropTetromino();
                            clearFullLines(); // Check and clear any full lines
                            createTetromino(getNextTetromino()); // Create a new Tetromino
                            tetrominoCount++;
                            break;
                        // Add other controls as needed
                    }
                    }
                    // Check for game over condition right after creating a new Tetromino
                    if (!canMove(currentTetromino.x, currentTetromino.y)) {
                        printf("Game Over\n");
                        writeScoreToFile(score);
                        break;
                    }
                    // Automatically move the Tetromino down or fix it if it can't move down
                    if (counter >= fallSpeed) {
                        if (canMove(currentTetromino.x, currentTetromino.y + 1)) {
                            moveTetromino(0, 1); // Move down
                        }
                        else {
                            fixTetromino();
                            clearFullLines();
                            //createTetromino(rand() % NUM_TETROMINOS); // Create a new Tetromino
                            createTetromino(getNextTetromino());
                            tetrominoCount++;
                        }
                        counter = 0; // Reset the counter
                    }
                    else {
                        counter++; // Increment the counter
                    }

                    printBoard(); // Print the updated board

                    _sleep(100); // Sleep for a delay (adjust as needed)
                }

                // After game ends, wait for 5 seconds
                _sleep(2000);  // Sleep for 5000 milliseconds (5 seconds)
                break;
            case 2:

                displayOptionsMenu();
                break;
            case 3:

                displayLeaderboard();
                char ch;
                do {
                    ch = getchar();  // Wait for user to press 'X'
                } while (toupper(ch) != 'X');
                break;
            case 4:

                printf("Exiting...\n");
                _sleep(1000);

                return 0; // Exit the program
            default:

                printf("Invalid choice!\n");
        }
    }

    return 0;
}
