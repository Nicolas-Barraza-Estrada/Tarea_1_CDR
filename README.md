# Tarea_1_CDR
# Cuatro en Linea

Este proyecto implementa una aplicación cliente-servidor para un juego de Cuatro en Linea. El servidor maneja el juego y las conexiones de los clientes, mientras que el cliente permite a los usuarios jugar contra el servidor.

## Archivos

- `client.cpp`: Código fuente del cliente.
- `server.cpp`: Código fuente del servidor.
- `Makefile`: Archivo Makefile para compilar ambos programas.

## Prerrequisitos

- g++ (GNU Compiler Collection)
- make (utilidad de compilación)

## Compilación

Para compilar ambos programas, abre una terminal y navega al directorio donde se encuentran los archivos fuente y el Makefile. Luego, ejecuta:

```sh
make

./server [puerto]
./client <server_ip> <server_port>


# Ejemplo
./server 5555
./client 127.0.0.1 5555
make clean
