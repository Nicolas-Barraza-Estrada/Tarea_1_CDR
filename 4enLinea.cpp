#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

#define ROWS 6
#define COLS 7
#define COMPUTERMOVE 'S'
#define HUMANMOVE 'C'

void showBoard(char board[][COLS]) {
    cout << "\n";
    for (int i = 0; i < ROWS; i++) {
        cout << "\t\t\t  ";
        for (int j = 0; j < COLS; j++) {
            cout << board[i][j];
            if (j < COLS - 1) cout << " | ";
        }
        if (i < ROWS - 1) cout << "\n\t\t\t---------------------\n";
    }
    cout << "\n\n";
}

void initialise(char board[][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
            board[i][j] = ' ';
    }
}

bool checkFour(char board[][COLS], char token) {
    // Horizontal check
    for (int i = 0; i < ROWS; i++)
        for (int j = 0; j < COLS - 3; j++)
            if (board[i][j] == token && board[i][j + 1] == token && board[i][j + 2] == token && board[i][j + 3] == token)
                return true;

    // Vertical check
    for (int i = 0; i < ROWS - 3; i++)
        for (int j = 0; j < COLS; j++)
            if (board[i][j] == token && board[i + 1][j] == token && board[i + 2][j] == token && board[i + 3][j] == token)
                return true;

    // Diagonal check
    for (int i = 0; i < ROWS - 3; i++) {
        for (int j = 0; j < COLS - 3; j++) {
            if (board[i][j] == token && board[i + 1][j + 1] == token && board[i + 2][j + 2] == token && board[i + 3][j + 3] == token)
                return true;
        }
        for (int j = 3; j < COLS; j++) {
            if (board[i][j] == token && board[i + 1][j - 1] == token && board[i + 2][j - 2] == token && board[i + 3][j - 3] == token)
                return true;
        }
    }

    return false;
}

bool gameOver(char board[][COLS]) {
    return checkFour(board, COMPUTERMOVE) || checkFour(board, HUMANMOVE);
}

void declareWinner(int whoseTurn) {
    if (whoseTurn == COMPUTERMOVE)
        cout << "COMPUTER has won\n";
    else
        cout << "HUMAN has won\n";
}

void computerMove(char board[][COLS]) {
    srand(time(NULL)); // Initialize random seed
    int r, c;
    do {
        r = rand() % ROWS;
        c = rand() % COLS;
    } while (board[r][c] != ' '); // Ensure the cell is empty
    board[r][c] = COMPUTERMOVE;
}

bool decideFirstMove() {
    cout << "Who should start the game? (C)lient or (S)erver: ";
    char response;
    cin >> response;
    return (response == 'C' || response == 'c'); // True for Client, False for Server
}

void playConnectFour() {
    char board[ROWS][COLS];
    initialise(board);
    showBoard(board);

    bool humanTurn = decideFirstMove(); // Decide who starts first
    cout << (humanTurn ? "Client starts the game.\n" : "Server starts the game.\n");

    int moveIndex = 0, x, y;

    while (!gameOver(board) && moveIndex < ROWS * COLS) {
        if (humanTurn) {
            cout << "Enter row and column to place " << HUMANMOVE << ": ";
            cin >> x >> y;
            if (x >= 0 && x < ROWS && y >= 0 && y < COLS && board[x][y] == ' ') {
                board[x][y] = HUMANMOVE;
                humanTurn = false; // Switch turn to computer
            } else {
                cout << "Invalid move, try again.\n";
                continue;
            }
        } else {
            computerMove(board);
            humanTurn = true; // Switch turn to human
        }
        showBoard(board);
        moveIndex++;
    }

    if (!gameOver(board) && moveIndex == ROWS * COLS)
        cout << "It's a draw\n";
    else {
        declareWinner(humanTurn ? COMPUTERMOVE : HUMANMOVE);
    }
}

int main() {
    playConnectFour();
    return 0;
}
