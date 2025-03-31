#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "DatabaseConnection.h"
#include <iomanip>
#include <conio.h>

void RunDatabaseTest() {
    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        // Test INSERT
        db.InsertProduct(L"Pizza Margherita", 29.99, 10);
        db.InsertProduct(L"Tiramisu", 15.50, 20);

        // Test SELECT
        auto results = db.ExecuteQuery(L"SELECT TOP 5 ID, Nume, Pret FROM Produse ORDER BY ID DESC");

        std::wcout << L"Latest products:\n";
        for (const auto& row : results) {
            for (const auto& col : row) {
                std::wcout << col << L" | ";
            }
            std::wcout << L"\n";
        }

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    std::cout << "\nPress any key to exit...";
    _getch();
}

int main(array<System::String^>^ args)
{
    RunDatabaseTest();
    system("pause");
    //WSADATA wsaData;

    //// Initializarea Winsock
    //int wsaInitResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    //if (wsaInitResult != 0) {
    //    std::cerr << "Eroare la initializarea Winsock: " << wsaInitResult << std::endl;
    //    return 1;
    //}

    //// Crearea unui socket pentru server
    //SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    //if (serverSocket == INVALID_SOCKET) {
    //    std::cerr << "Eroare la crearea socketului server: " << WSAGetLastError() << std::endl;
    //    WSACleanup();
    //    return 1;
    //}

    //// Configurarea adresei serverului
    //sockaddr_in serverAddr;
    //serverAddr.sin_family = AF_INET;
    //serverAddr.sin_addr.s_addr = INADDR_ANY;  // Accepta conexiuni de la orice adresa IP
    //serverAddr.sin_port = htons(12345); // Portul pe care serverul va asculta

    //// Legarea socketului la adresa si port
    //int bindResult = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    //if (bindResult == SOCKET_ERROR) {
    //    std::cerr << "Eroare la bind: " << WSAGetLastError() << std::endl;
    //    closesocket(serverSocket);
    //    WSACleanup();
    //    return 1;
    //}

    //// Ascultarea pentru conexiuni
    //int listenResult = listen(serverSocket, SOMAXCONN);
    //if (listenResult == SOCKET_ERROR) {
    //    std::cerr << "Eroare la listen: " << WSAGetLastError() << std::endl;
    //    closesocket(serverSocket);
    //    WSACleanup();
    //    return 1;
    //}

    //std::cout << "Serverul asteapta conexiuni...\n";

    //// Acceptarea conexiunii clientului
    //SOCKET clientSocket = accept(serverSocket, NULL, NULL);
    //if (clientSocket == INVALID_SOCKET) {
    //    std::cerr << "Eroare la acceptarea conexiunii: " << WSAGetLastError() << std::endl;
    //    closesocket(serverSocket);
    //    WSACleanup();
    //    return 1;
    //}

    //std::cout << "Client conectat!\n";

    //char buffer[1024];
    //while (true) {
    //    memset(buffer, 0, sizeof(buffer));  // Curata buffer-ul

    //    // Primeste date de la client
    //    int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    //    if (bytesReceived == SOCKET_ERROR) {
    //        std::cerr << "Eroare la receptionarea datelor: " << WSAGetLastError() << std::endl;
    //        break;
    //    }

    //    if (bytesReceived == 0) {
    //        std::cout << "Clientul s-a deconectat.\n";
    //        break;
    //    }

    //    // Afiseaza mesajul primit de la client
    //    std::cout << "Client: " << buffer << std::endl;

    //    // Trimite un raspuns catre client
    //    std::string response = "Serverul a primit: " + std::string(buffer);
    //    int sentBytes = send(clientSocket, response.c_str(), response.length(), 0);
    //    if (sentBytes == SOCKET_ERROR) {
    //        std::cerr << "Eroare la trimiterea mesajului: " << WSAGetLastError() << std::endl;
    //        break;
    //    }
    //    else {
    //        std::cout << "Mesaj trimis clientului: " << response << std::endl;
    //    }
    //}

    //// Inchide socketurile
    //closesocket(clientSocket);
    //closesocket(serverSocket);
    //WSACleanup();

    return 0;
}