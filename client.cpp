#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <poll.h>

#define BUFFER_SIZE 1024

using namespace std;

void displayBoard(const string& boardStr) {
    cout << boardStr;
}

int main(int argc, char **argv) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <server_ip> <server_port>" << endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int server_port = atoi(argv[2]);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cerr << "Error creating socket" << endl;
        return 1;
    }

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(server_port); // Puerto del servidor

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported" << endl;
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed" << endl;
        return 1;
    }

    struct pollfd fds[2];
    fds[0].fd = sock;
    fds[0].events = POLLIN;
    fds[1].fd = fileno(stdin);
    fds[1].events = POLLIN;

    char buffer[BUFFER_SIZE]; // Buffer para recibir mensajes del servidor
    while (true) {
        int poll_count = poll(fds, 2, -1);
        if (poll_count < 0) {
            cerr << "Poll error" << endl;
            break;
        }

        if (fds[0].revents & POLLIN) {
            int n_bytes = recv(sock, buffer, BUFFER_SIZE - 1, 0);
            if (n_bytes <= 0) {
                cout << "Server closed connection or error occurred" << endl;
                break;
            }
            buffer[n_bytes] = '\0'; 
            string response(buffer);
            displayBoard(response); // Mostrar el tablero del juego

            if (response.find("Game Over") != string::npos) {
                break;
            }
        }

        if (fds[1].revents & POLLIN) {
            string input;
            getline(cin, input);
            send(sock, input.c_str(), input.length(), 0); // Enviar la entrada del usuario al servidor

            if (input == "Q") {
                break;
            }
        }
    }

    close(sock);
    return 0;
}
