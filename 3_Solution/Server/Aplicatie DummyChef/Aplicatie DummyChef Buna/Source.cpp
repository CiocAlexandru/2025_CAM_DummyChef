#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

int main()
{
    WSADATA wsaData;

    // Initializarea Winsock
    int wsaInitResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaInitResult != 0) {
        std::cerr << "Eroare la initializarea Winsock: " << wsaInitResult << std::endl;
        return 1;
    }

    // Crearea unui socket pentru server
    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        std::cerr << "Eroare la crearea socketului server: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // Configurarea adresei serverului
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Accepta conexiuni de la orice adresa IP
    serverAddr.sin_port = htons(12345); // Portul pe care serverul va asculta

    // Legarea socketului la adresa si port
    int bindResult = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bindResult == SOCKET_ERROR) {
        std::cerr << "Eroare la bind: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // Ascultarea pentru conexiuni
    int listenResult = listen(serverSocket, SOMAXCONN);
    if (listenResult == SOCKET_ERROR) {
        std::cerr << "Eroare la listen: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Serverul asteapta conexiuni...\n";

    // Acceptarea conexiunii clientului
    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket == INVALID_SOCKET) {
        std::cerr << "Eroare la acceptarea conexiunii: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::cout << "Client conectat!\n";

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));  // Curata buffer-ul

        // Primeste date de la client
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Eroare la receptionarea datelor: " << WSAGetLastError() << std::endl;
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "Clientul s-a deconectat.\n";
            break;
        }

        // Afiseaza mesajul primit de la client
        std::cout << "Client: " << buffer << std::endl;

        // Trimite un raspuns catre client
        std::string response = "Serverul a primit: " + std::string(buffer);
        int sentBytes = send(clientSocket, response.c_str(), response.length(), 0);
        if (sentBytes == SOCKET_ERROR) {
            std::cerr << "Eroare la trimiterea mesajului: " << WSAGetLastError() << std::endl;
            break;
        }
        else {
            std::cout << "Mesaj trimis clientului: " << response << std::endl;
        }
    }

    // Inchide socketurile
    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}