# Nombre del compilador
CXX = g++

# Flags del compilador
CXXFLAGS = -Wall -Wextra -std=c++11 -pthread

# Nombre de los ejecutables
CLIENT_TARGET = client
SERVER_TARGET = server

# Archivos fuente
CLIENT_SRCS = 4enLineaCliente.cpp
SERVER_SRCS = 4enLineaServer.cpp

# Archivos objeto (se derivan automáticamente de los archivos fuente)
CLIENT_OBJS = $(CLIENT_SRCS:.cpp=.o)
SERVER_OBJS = $(SERVER_SRCS:.cpp=.o)

# Regla por defecto: compilar todo
all: $(CLIENT_TARGET) $(SERVER_TARGET)

# Regla para enlazar el ejecutable del cliente
$(CLIENT_TARGET): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $(CLIENT_TARGET) $(CLIENT_OBJS)

# Regla para enlazar el ejecutable del servidor
$(SERVER_TARGET): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $(SERVER_TARGET) $(SERVER_OBJS)

# Regla para compilar archivos fuente en archivos objeto (cliente)
client.o: client.cpp
	$(CXX) $(CXXFLAGS) -c client.cpp -o client.o

# Regla para compilar archivos fuente en archivos objeto (servidor)
server.o: server.cpp
	$(CXX) $(CXXFLAGS) -c server.cpp -o server.o

# Regla para limpiar archivos generados
clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_TARGET) $(SERVER_TARGET)

.PHONY: all clean
