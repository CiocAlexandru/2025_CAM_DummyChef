#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")



int main()
{
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    listen(serverSocket, SOMAXCONN);
    std::cout << "Serverul asteapta conexiuni...\n";

    SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    std::cout << "Client conectat!\n";

    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived <= 0) break;
        std::cout << "Client: " << buffer << std::endl;

        // Trimite raspuns cãtre client
        std::string response = "Serverul a primit: " + std::string(buffer);
        int sentBytes = send(clientSocket, response.c_str(), response.length(), 0);

        if (sentBytes == SOCKET_ERROR) {
            std::cerr << "Eroare la trimiterea mesajului: " << WSAGetLastError() << std::endl;
        }
        else {
            std::cout << "Mesaj trimis clientului: " << response << std::endl;
        }
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();
	return 0;
}