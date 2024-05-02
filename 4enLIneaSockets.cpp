#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define ROWS 6
#define COLS 7
#define COMPUTERMOVE 'O'
#define HUMANMOVE 'X'

using namespace std;

void showBoard(char board[][COLS], int client_sock) {
    string display = "\n";
    for (int i = 0; i < ROWS; i++) {
        display += "\t\t\t  ";
        for (int j = 0; j < COLS; j++) {
            display += board[i][j];
            if (j < COLS - 1) display += " | ";
        }
        if (i < ROWS - 1) display += "\n\t\t\t---------------------\n";
    }
    display += "\n\n";
    send(client_sock, display.c_str(), display.length(), 0);
}

void initialise(char board[][COLS]) {
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++)
            board[i][j] = ' ';
    }
}

bool checkFour(char board[][COLS], char token) {
    // Horizontal check
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS - 3; j++) {
            if (board[i][j] == token && board[i][j + 1] == token && board[i][j + 2] == token && board[i][j + 3] == token)
                return true;
        }
    }
    // Vertical check
    for (int j = 0; j < COLS; j++) {
        for (int i = 0; i < ROWS - 3; i++) {
            if (board[i][j] == token && board[i + 1][j] == token && board[i + 2][j] == token && board[i + 3][j] == token)
                return true;
        }
    }
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

void* playGame(void* arg) {
    int client_sock = *((int*)arg);
    char board[ROWS][COLS];
    initialise(board);
    showBoard(board, client_sock);

    char buffer[1024];
    int n_bytes;

    while ((n_bytes = recv(client_sock, buffer, 1024, 0)) > 0) {
        buffer[n_bytes] = '\0';  // Null-terminate string
        int column = atoi(buffer) - 1;  // Convert buffer to column index

        if (column >= 0 && column < COLS) {
            // Drop token in the selected column
            for (int i = ROWS - 1; i >= 0; i--) {
                if (board[i][column] == ' ') {
                    board[i][column] = HUMANMOVE;
                    break;
                }
            }
        }

        showBoard(board, client_sock);
        if (gameOver(board)) {
            char winMsg[] = "Game Over: You win!";
            send(client_sock, winMsg, sizeof(winMsg), 0);
            break;
        }

        // Computer's move
        int compColumn = rand() % COLS;
        for (int i = ROWS - 1; i >= 0; i--) {
            if (board[i][compColumn] == ' ') {
                board[i][compColumn] = COMPUTERMOVE;
                break;
            }
        }
        
        showBoard(board, client_sock);
        if (gameOver(board)) {
            char loseMsg[] = "Game Over: Computer wins!";
            send(client_sock, loseMsg, sizeof(loseMsg), 0);
            break;
        }
    }

    close(client_sock);
    delete (int*)arg;
    return NULL;
}

int main(int argc, char **argv) {
    int port = 12345;
    if (argc > 1) port = atoi(argv[1]);

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);

    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Error binding socket" << endl;
        return 1;
    }

    if (listen(server_sock, 5) < 0) {
        cerr << "Error listening" << endl;
        return 1;
    }

    cout << "Server listening on port " << port << endl;

    while (true) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_sock < 0) {
            cerr << "Error accepting connection" << endl;
            continue;
        }

        int* pclient = new int(client_sock);
        pthread_t tid;
        pthread_create(&tid, NULL, playGame, pclient);
        pthread_detach(tid);
    }

    close(server_sock);
    return 0;
}
