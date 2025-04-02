#include "DummyChef.h"
#include <conio.h>
#include <iomanip>






DummyChef* DummyChef::app = nullptr;


DummyChef* DummyChef::getApp(Administrator* administrator)
{
	if (app == nullptr)
	{
		app = new DummyChef(administrator);
	}
	return app;
}

void DummyChef::destroyApp()
{
	if (app != nullptr)
	{
		delete app;
		app = nullptr;
	}
}



void DummyChef::run()
{
   
    try {
        RunDatabaseTest();
        InsertNewClient();
        connectToClient();
    }
    catch (...) {
        closeSocket(); // Asigura-te că socket-urile sunt inchise in caz de eroare
        throw; // Re-arunca exceptia pentru gestionare ulterioara
    }




    
    closeSocket();
}

void DummyChef::InsertNewClient() {
    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        db.InsertClient(L"Popescu", L"Ion", L"ionpopescu", L"parola123",
            L"0745123456", L"1990-05-15", L"ion.popescu@example.com",
            L"Strada Mihai Eminescu, nr. 10, Bucuresti");

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error inserting client: " << e.what() << std::endl;
    }
}


void DummyChef::RunDatabaseTest()
{
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



void DummyChef::connectToClient()
{
    WSADATA wsaData;

    // Initializarea Winsock
    int wsaInitResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaInitResult != 0) 
    {
      
        throw new ExceptieSocket(wsaInitResult, "Eroare la initializarea Winsock, linia: ",86);
    }

    // Crearea unui socket pentru server
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) 
    {
        WSACleanup();
        throw new ExceptieSocket(WSAGetLastError(), "Eroare la crearea socketului server, linia: ", 94);
    }

    // Configurarea adresei serverului
    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;  // Accepta conexiuni de la orice adresa IP
    serverAddr.sin_port = htons(12345); // Portul pe care serverul va asculta

    // Legarea socketului la adresa si port
    int bindResult = bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr));
    if (bindResult == SOCKET_ERROR) 
    {
        closesocket(serverSocket);
        WSACleanup();
        throw new ExceptieSocket(WSAGetLastError(), "Eroare la bind, linia: ", 109);
    }

    // Ascultarea pentru conexiuni
    int listenResult = listen(serverSocket, SOMAXCONN);
    if (listenResult == SOCKET_ERROR) 
    {
        closesocket(serverSocket);
        WSACleanup();
        throw new ExceptieSocket(WSAGetLastError(), "Eroare la listen, linia: ", 118);
    }

    std::cout << "Serverul asteapta conexiuni...\n";

    // Acceptarea conexiunii clientului
    sockaddr_in clientAddr;
    int clientAddrSize = sizeof(clientAddr);
    clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);
    if (clientSocket == INVALID_SOCKET) 
    {
        closesocket(serverSocket);
        WSACleanup();
        throw new ExceptieSocket(WSAGetLastError(), "Eroare la acceptarea conexiunii, linia: ", 129);
    }

    std::cout << "Client conectat!\n";

    char buffer[1024];
    while (true) 
    {
        memset(buffer, 0, sizeof(buffer));  // Curata buffer-ul

        // Primeste date de la client
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR) 
        {
            throw new ExceptieSocket(WSAGetLastError(), "Eroare la receptionarea datelor, linia: ", 143);
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
        if (sentBytes == SOCKET_ERROR) 
        {
            throw new ExceptieSocket(WSAGetLastError(), "Eroare la trimiterea mesajului, linia: ", 159);
        }
        else {
            std::cout << "Mesaj trimis clientului: " << response << std::endl;
        }
    }
}

void DummyChef::closeSocket()
{
    // Inchide socketurile
    if (clientSocket != INVALID_SOCKET) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    if (serverSocket != INVALID_SOCKET) {
        closesocket(serverSocket);
        serverSocket = INVALID_SOCKET;
    }

    WSACleanup();
}



void DummyChef::handleLogin(const std::string& email, const std::string& password) {
    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        auto user = db.GetUserByCredentials(
            converter.from_bytes(email),
            converter.from_bytes(password)
        );

        if (user) {
            std::cout << "Login successful! Welcome " << user->getNume() << std::endl;
            // Poți adăuga utilizatorul în vectorul de utilizatori
            utilizatori.push_back(user.release());
        }
        else {
            std::cout << "Login failed! Invalid credentials." << std::endl;
        }

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error during login: " << e.what() << std::endl;
    }
}