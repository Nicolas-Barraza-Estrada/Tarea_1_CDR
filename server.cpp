#include <iostream>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <netdb.h>


#define ROWS 6
#define COLS 7
#define COMPUTERMOVE 'S'
#define HUMANMOVE 'C'

using namespace std;

class Board {
private:
    char board[ROWS][COLS];

public:
    Board() {
        initialise();
    }

    void initialise() {
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                board[i][j] = ' ';
            }
        }
    }

    void showBoard(int client_sock) {
        string display = "\n";
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS; j++) {
                display += board[i][j] == ' ' ? ". " : string(1, board[i][j]) + " ";
            }
            display += "\n";
        }
        display += "Enter the column number (1-7) to place your disc:\n";
        send(client_sock, display.c_str(), display.length(), 0);
    }

    bool makeMove(int column, char token) {
        for (int i = ROWS - 1; i >= 0; i--) {
            if (board[i][column] == ' ') {
                board[i][column] = token;
                return true;
            }
        }
        return false;
    }

    bool checkFour(char token) {
        // Horizontal, vertical, and diagonal checks
        for (int i = 0; i < ROWS; i++) {
            for (int j = 0; j < COLS - 3; j++) {
                if (board[i][j] == token && board[i][j + 1] == token &&
                    board[i][j + 2] == token && board[i][j + 3] == token) {
                    return true;
                }
            }
        }
        for (int j = 0; j < COLS; j++) {
            for (int i = 0; i < ROWS - 3; i++) {
                if (board[i][j] == token && board[i + 1][j] == token &&
                    board[i + 2][j] == token && board[i + 3][j] == token) {
                    return true;
                }
            }
        }
        for (int i = 0; i < ROWS - 3; i++) {
            for (int j = 0; j < COLS - 3; j++) {
                if (board[i][j] == token && board[i + 1][j + 1] == token &&
                    board[i + 2][j + 2] == token && board[i + 3][j + 3] == token) {
                    return true;
                }
                if (board[i][j + 3] == token && board[i + 1][j + 2] == token &&
                    board[i + 2][j + 1] == token && board[i + 3][j] == token) {
                    return true;
                }
            }
        }
        return false;
    }

    bool gameOver() {
        return checkFour(COMPUTERMOVE) || checkFour(HUMANMOVE);
    }
};

class Game {
private:
    Board board;
    int client_sock;
    const char* client_ip;
    int client_port;

public:
    Game(int client_sock, const char* client_ip, int client_port) {
        this->client_sock = client_sock;
        this->client_ip = client_ip;
        this->client_port = client_port;
        cout << "Juego nuevo [" << client_ip << ":" << client_port << "]" << endl;
    }

    void startGame(){
        srand(time(NULL));
        
        bool computerStarts = rand() % 2 == 0;

        if (computerStarts) {
            cout << "Juego [" << client_ip << ":" << client_port << "]: inicia juego el servidor." << endl;
            computerMove();
        } else {
            cout << "Juego [" << client_ip << ":" << client_port << "]: inicia juego el cliente." << endl;
            board.showBoard(client_sock);
        }

        receiveMoves();
    }

    void computerMove() {
        bool validMove = false;
        int column;
        do {
            column = rand() % COLS;
            validMove = board.makeMove(column, COMPUTERMOVE);
        } while (!validMove);
        cout << "Juego [" << client_ip << ":" << client_port << "]: el servidor juega en la columna " << column + 1 << "." << endl;
        
        board.showBoard(client_sock);
        if (board.gameOver()) {
            const char* loseMsg = "Game Over: Computer wins!\n";
            cout << "Juego [" << client_ip << ":" << client_port << "]: el servidor gana." << endl;
            send(client_sock, loseMsg, strlen(loseMsg), 0);
            close(client_sock);
        }
    }

    void receiveMoves() {
        char buffer[1024];
        int n_bytes;

        while ((n_bytes = recv(client_sock, buffer, 1024, 0)) > 0) {
            buffer[n_bytes] = '\0';
            int column = atoi(buffer) - 1;
            if (buffer[0] == 'Q') {
                const char* quitMsg = "Game Over: You quit the game.\n";
                send(client_sock, quitMsg, strlen(quitMsg), 0);
                break;
            }
            if (column >= 0 && column < COLS && board.makeMove(column, HUMANMOVE)) {
                cout << "Juego [" << client_ip << ":" << client_port << "]: el cliente juega en la columna " << column + 1 << "." << endl;
                board.showBoard(client_sock);
                if (board.gameOver()) {
                    const char* winMsg = "Game Over: You win!\n";
                    cout << "Juego [" << client_ip << ":" << client_port << "]: el cliente gana." << endl;
                    send(client_sock, winMsg, strlen(winMsg), 0);
                    break;
                }
                computerMove();
            } else {
                const char* errMsg = "Invalid input or column full. Please enter a column number from 1 to 7.\n";
                send(client_sock, errMsg, strlen(errMsg), 0);
            }
        }

        close(client_sock);
    }
};

void* server_thread(void* arg) {
    int client_sock = *((int*)arg);

    struct sockaddr_in client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    if (getpeername(client_sock, (struct sockaddr *)&client_addr, &client_addr_len) == -1) {
        cerr << "Error obteniendo la dirección IP del cliente" << endl;
        return NULL;
    }
    
    char *client_ip = inet_ntoa(client_addr.sin_addr);
    int client_port = ntohs(client_addr.sin_port);

    Game* game = new Game(client_sock, client_ip, client_port);
    game->startGame();
    delete game;
    cout << "Juego terminado [" << client_ip << ":" << client_port << "]" << endl;
    return NULL;
}

int main(int argc, char **argv) {
    int port = 12345;
    if (argc > 1) {
        port = atoi(argv[1]);
    }

    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    sockaddr_in server_addr;
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

    char hostname[1024];
    gethostname(hostname, 1024);
    struct hostent* host_info = gethostbyname(hostname);
    char* server_ip = inet_ntoa(*(struct in_addr*)host_info->h_addr);
    cout << "Server IP address: " << server_ip << endl;



    cout << "Server listening on port " << port << endl;
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr);

    while (true) {
        int client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
        if (client_sock < 0) {
            cerr << "Error accepting connection" << endl;
            continue;
        }

        pthread_t tid;
        pthread_create(&tid, NULL, server_thread, new int(client_sock));
        pthread_detach(tid);
    }

    close(server_sock);
    return 0;
}
