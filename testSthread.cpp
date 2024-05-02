#include <sys/socket.h> // socket()
#include <arpa/inet.h>  // hton*()
#include <string.h>     // memset()
#include <unistd.h> 
#include <iostream>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>

using namespace std;

void* jugar(void* arg) {
    int socket_cliente = *((int*)arg);
    struct sockaddr_in direccionCliente;
    socklen_t addr_size = sizeof(direccionCliente);
    
    // Obtener la información del cliente
    getpeername(socket_cliente, (struct sockaddr*)&direccionCliente, &addr_size);
    
    //
    char buffer[1024];
    memset(buffer, '\0', sizeof(char)*1024);
    int n_bytes = 0;
        
    //
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(direccionCliente.sin_addr), ip, INET_ADDRSTRLEN);
    //
    cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Nuevo jugador." << endl;
    
    //crear array para guardar los datos recibidos del cliente
    char datos[1024];
    int contador = 0;
    memset(datos, '\0', sizeof(char)*1024);

    //
    while ((n_bytes = recv(socket_cliente, buffer, 1024, 0))) {
        buffer[n_bytes] = '\0';
                
        //
        if (buffer[0] == 'Q') {
            cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Sale del juego." << endl;
            close(socket_cliente);
            break;
        }
                
        //
        switch (buffer[0]) {
            case 'C':       // C columna
                {
                string line(&buffer[0]);
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Columna: " << line[2] << endl;
                // guardar los datos recibidos del cliente
                datos[contador] = line[2];
                contador++;
                send(socket_cliente, "ok\n", 3, 0);
                break;
                }
            case 'R':       //mostrar los datos
                {
                cout << "[" << ip << ":" << ntohs(direccionCliente.sin_port) << "] Datos: ";    
                for (int i = 0; i < contador; i++) {
                    cout << datos[i] << " ";
                }
                cout << endl;
                // enviar los datos al cliente
                send(socket_cliente, datos , contador, 0);
                break;
                }

            default:
                // instrucción no reconocida.
                send(socket_cliente, "error\n", 6, 0);
        }
    }
    
    // Liberar la memoria asignada al argumento
    delete (int*)arg;
    return NULL;
}

int main(int argc, char **argv) {
    //
    int port = atoi(argv[1]);
    int socket_server = 0;
    // socket address structures.
    struct sockaddr_in direccionServidor, direccionCliente;   
    
    // crea el socket.
    cout << "Creating listening socket ...\n";
    if ((socket_server = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cout << "Error creating listening socket\n";
        exit(EXIT_FAILURE);
    }
    
    // configuracion de los atributos de la clase sockaddr_in.
    cout << "Configuring socket address structure ...\n";
    memset(&direccionServidor, 0, sizeof(direccionServidor));
    direccionServidor.sin_family      = AF_INET;
    direccionServidor.sin_addr.s_addr = htonl(INADDR_ANY);
    direccionServidor.sin_port        = htons(port);
    
    //
    cout << "Binding socket ...\n";
    if (bind(socket_server, (struct sockaddr *) &direccionServidor, sizeof(direccionServidor)) < 0) {
        cout << "Error calling bind()\n";
        exit(EXIT_FAILURE);
    }
    
    //
    cout << "Calling listening ...\n";
    if (listen(socket_server, 1024) < 0) {
        cout << "Error calling listen()\n";
        exit(EXIT_FAILURE);
    }
    
    // para obtener info del cliente.
    socklen_t addr_size;
    addr_size = sizeof(struct sockaddr_in);
    
    //
    cout << "Waiting client request ...\n";
    while (true) {
        /*  Wait for a connection, then accept() it  */
        int* socket_cliente = new int;
        
        //
        if ((*socket_cliente = accept(socket_server, (struct sockaddr *)&direccionCliente, &addr_size)) < 0) {
            cout << "Error calling accept()\n";
            delete socket_cliente;
            exit(EXIT_FAILURE);
        }
        
        //
        pthread_t thread_id;
        if (pthread_create(&thread_id, NULL, jugar, (void*)socket_cliente) < 0) {
            cout << "Error creating thread\n";
            delete socket_cliente;
            exit(EXIT_FAILURE);
        }
        
        // Detach the thread to avoid memory leaks
        pthread_detach(thread_id);
    }
    
    //
    return 0;
}
