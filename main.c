#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <conio.h> // For _kbhit() and _getch()

#define BOARD_WIDTH 10
#define BOARD_HEIGHT 20

// Representing the game board
int board[BOARD_HEIGHT][BOARD_WIDTH] = {0};

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
    for (int y = 0; y < BOARD_HEIGHT; y++) {
        for (int x = 0; x < BOARD_WIDTH; x++) {
            printf("%c ", board[y][x] ? '#' : '.');
        }
        printf("\n");
    }
}

int main() {
    initBoard();

    while (true) {
        // Game logic here

        // Example: print the board
        printBoard();

        // Handle user input
        if (_kbhit()) {
            char key = _getch();
            // Move Tetrominoes based on user input
        }

        // Add a delay
        _sleep(1000); // Sleep for 1000 milliseconds
    }

    return 0;
}
