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

        InsertNewProduct();
        registerUser("Client", "Popescu", "Ion", "ionpopescu33546", "parola123",
            "0745123456", "1990-05-15", "ion.popescu@example.com", "Strada Mihai Eminescu, nr. 10, Bucuresti");
        registerUser("Bucatar", "Ionescu", "Maria", "mariaionescu", "chef123",
    "0723123456", "1985-08-20", "maria.ionescu@example.com", "", 5, "https://example.com/chef-demo");
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


void DummyChef::InsertNewProduct()
{
    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();
        // Test INSERT
        db.InsertProduct(L"Pizza Margherita", 29.99, 10);
        db.InsertProduct(L"Tiramisu", 15.50, 20);
        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
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
            // If there's already a logged-in user, log them out first
            if (this->utilizator != nullptr) {
                delete this->utilizator;
                this->utilizator = nullptr;
            }

            // Set the new user as current
            this->utilizator = user.release();
            std::cout << "Login successful! Welcome " << this->utilizator->getNume() << std::endl;
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

void DummyChef::registerUser(const std::string& userType, const std::string& nume, const std::string& prenume,
    const std::string& nume_utilizator, const std::string& parola, const std::string& nr_telefon,
    const std::string& data_nasterii, const std::string& email,
    const std::string& adresa_livrare , int experienta, const std::string& link_demonstrativ )
{
    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        // Convert strings to wstrings for database compatibility
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wNume = converter.from_bytes(nume);
        std::wstring wPrenume = converter.from_bytes(prenume);
        std::wstring wNumeUtilizator = converter.from_bytes(nume_utilizator);
        std::wstring wParola = converter.from_bytes(parola);
        std::wstring wNrTelefon = converter.from_bytes(nr_telefon);
        std::wstring wDataNasterii = converter.from_bytes(data_nasterii);
        std::wstring wEmail = converter.from_bytes(email);
        std::wstring wAdresaLivrare = converter.from_bytes(adresa_livrare);
        std::wstring wLinkDemonstrativ = converter.from_bytes(link_demonstrativ);

        // Check if user already exists
        if (db.UserExists(wEmail)) {
            std::cout << "Error: A user with this email already exists!" << std::endl;
            db.Disconnect();
            return;
        }

        if (userType == "Client") {
            db.InsertClient(wNume, wPrenume, wNumeUtilizator, wParola, wNrTelefon,
                wDataNasterii, wEmail, wAdresaLivrare);
            std::cout << "Client registered successfully!" << std::endl;
        }
        else if (userType == "Bucatar") {
            db.InsertChef(wNume, wPrenume, wNumeUtilizator, wParola, wNrTelefon,
                wDataNasterii, wEmail, experienta, wLinkDemonstrativ);
            std::cout << "Chef registered successfully!" << std::endl;
        }
        else {
            std::cout << "Error: Invalid user type specified!" << std::endl;
        }

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error during registration: " << e.what() << std::endl;
    }
}

std::string DummyChef::forgotPassword(const std::string& email) {
    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wEmail = converter.from_bytes(email);

        std::wstring password = db.GetPasswordByEmail(wEmail);

        db.Disconnect();

        if (!password.empty()) {
            return converter.to_bytes(password);
        }
        return ""; // Return empty string if not found
    }
    catch (const std::exception& e) {
        std::cerr << "Error in forgotPassword: " << e.what() << std::endl;
        return "";
    }
}