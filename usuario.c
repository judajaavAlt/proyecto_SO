// cliente.c para Windows
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET clientSocket;
    struct sockaddr_in serverAddr;
    int addrLen = sizeof(struct sockaddr_in);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error al inicializar Winsock");
        return EXIT_FAILURE;
    }

    if ((clientSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Error al crear el socket del cliente");
        WSACleanup();
        return EXIT_FAILURE;
    }

    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serverAddr.sin_port = htons(PORT);

    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Error al conectar con el servidor");
        closesocket(clientSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Conectado al servidor %s:%d\n", SERVER_IP, PORT);

    char command[BUFFER_SIZE];

    while (1) {
        // Solicitar al usuario que ingrese un comando
        printf("Ingrese un comando (salida para salir): ");
        fgets(command, BUFFER_SIZE, stdin);

        // Enviar el comando al servidor
        send(clientSocket, command, strlen(command), 0);

        // Salir si el comando es "salida"
        if (strncmp(command, "salida", 6) == 0) {
            break;
        }

        // Recibir y mostrar la respuesta del servidor
        char buffer[BUFFER_SIZE];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            perror("Error al recibir datos del servidor");
            break;
        }

        buffer[bytesRead] = '\0';  // Asegurar que la cadena esté terminada correctamente
        printf("Respuesta del servidor:\n%s\n", buffer);
    }

    // Cerrar el socket y limpiar Winsock
    closesocket(clientSocket);
    WSACleanup();

    return 0;
}