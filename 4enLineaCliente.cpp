#include <iostream>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>

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
    server_addr.sin_port = htons(server_port);

    if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
        cerr << "Invalid address/ Address not supported" << endl;
        return 1;
    }

    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        cerr << "Connection failed" << endl;
        return 1;
    }

    char buffer[BUFFER_SIZE];
    while (true) {
        int n_bytes = recv(sock, buffer, BUFFER_SIZE, 0);
        if (n_bytes <= 0) {
            break;
        }
        buffer[n_bytes] = '\0';
        string response(buffer);
        displayBoard(response);

        if (response.find("Game Over") != string::npos) {
            break;
        }

        string input;
        getline(cin, input);
        send(sock, input.c_str(), input.length(), 0);

        if (input == "Q") {
            break;
        }
    }

    close(sock);
    return 0;
}
