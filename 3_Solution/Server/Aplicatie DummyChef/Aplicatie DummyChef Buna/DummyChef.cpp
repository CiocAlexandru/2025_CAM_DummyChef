#include "DummyChef.h"
#include <conio.h>
#include <iomanip>
#include <sstream>
#include "Client.h"
#include "Bucatar.h"






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
}



void DummyChef::connectToClient()
{

    WSADATA wsaData;
    int wsaInitResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaInitResult != 0) {
        std::cerr << "Eroare Winsock: " << wsaInitResult << std::endl;
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        std::cerr << "Eroare la crearea socketului server." << std::endl;
        return;
    }

    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(12345);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        std::cerr << "Eroare la bind." << std::endl;
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        std::cerr << "Eroare la listen." << std::endl;
        return;
    }

    std::cout << "Serverul asteapta conexiuni...\n";

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Eroare la acceptarea conexiunii.\n";
            continue;
        }

        std::cout << "Client conectat!\n";

        handleClient(); // Tot fără thread

        closesocket(clientSocket);
    }

    closeSocket();
}


void DummyChef::handleClient()
{
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesReceived == SOCKET_ERROR) {
            std::cerr << "Eroare la receptionare. Inchidem clientul." << std::endl;
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "Clientul s-a deconectat.\n";
            break;
        }

        std::string receivedMessage(buffer);
        std::cout << "Client: " << receivedMessage << std::endl;

        try {
            if (receivedMessage.rfind("LOGIN ", 0) == 0) {
                std::istringstream iss(receivedMessage);
                std::string command, email, password;
                iss >> command >> email >> password;

                if (handleLogin(email, password)) {
                    std::string response = "LOGIN_SUCCESS";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else {
                    std::string response = "LOGIN_FAILED";
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
            }
            else if (receivedMessage.rfind("SIGNUP ", 0) == 0) {
                std::istringstream iss(receivedMessage);
                std::string command, userType;
                iss >> command >> userType;

                if (userType == "CLIENT") {
                    std::string nume, prenume, username, parola, telefon, data_nasterii, email, adresa;
                    iss >> nume >> prenume >> username >> parola >> telefon >> data_nasterii >> email >> adresa;

                    registerUser("Client", nume, prenume, username, parola, telefon, data_nasterii, email, adresa);
                    std::string response = "SIGNUP_CLIENT_SUCCESS";
                    send(clientSocket, response.c_str(), response.size(), 0);
                }
                else if (userType == "CHEF") {
                    std::string nume, prenume, username, parola, telefon, data_nasterii, email, link_demo;
                    int experienta;
                    iss >> nume >> prenume >> username >> parola >> telefon >> data_nasterii >> email >> experienta >> link_demo;

                    registerUser("Bucatar", nume, prenume, username, parola, telefon,
                        data_nasterii, email, "", experienta, link_demo);
                    std::string response = "SIGNUP_CHEF_SUCCESS";
                    send(clientSocket, response.c_str(), response.size(), 0);
                }
                else {
                    send(clientSocket, "REGISTER_INVALID_TYPE", 22, 0);
                }
            }
            else if (receivedMessage.rfind("FORGOT_PASSWORD ", 0) == 0) {
                handleForgotPassword(receivedMessage);
            }
            else if (receivedMessage.rfind("RESET_PASSWORD ", 0) == 0) {
                handleResetPassword(receivedMessage);
            }
            else if (receivedMessage.rfind("PREFERINTE_CLIENT ", 0) == 0)
            {
                handleClientPreferences(receivedMessage);
            }
            else {
                send(clientSocket, "UNKNOWN_COMMAND", 16, 0);
            }
            
        }
        catch (const std::exception& e) {
            std::string error = "SERVER_ERROR: " + std::string(e.what());
            send(clientSocket, error.c_str(), error.length(), 0);
        }
    }

    closesocket(clientSocket);
    clientSocket = INVALID_SOCKET;
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


bool DummyChef::handleLogin(const std::string& email, const std::string& password) {
    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        auto user = db.GetUserByCredentials(
            converter.from_bytes(email),
            converter.from_bytes(password)
        );

        if (user) {
            if (this->utilizator != nullptr) {
                delete this->utilizator;
                this->utilizator = nullptr;
            }

            this->utilizator = user.release();
            std::cout << "Login successful! Welcome " << this->utilizator->getNume() << std::endl;

            // Determină tipul utilizatorului prin dynamic_cast
            std::string response;
            if (dynamic_cast<Client*>(this->utilizator)) {
                response = "LOGIN_SUCCESS_CLIENT";
            }
            else if (dynamic_cast<Bucatar*>(this->utilizator)) {
                response = "LOGIN_SUCCESS_CHEF";
            }
            else {
                response = "LOGIN_FAILED";
            }

            send(clientSocket, response.c_str(), response.length(), 0);
            db.Disconnect();
            return true;
        }
        else {
            std::cout << "Login failed! Invalid credentials." << std::endl;
            std::string response = "LOGIN_FAILED";
            send(clientSocket, response.c_str(), response.length(), 0);
            db.Disconnect();
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error during login: " << e.what() << std::endl;
        return false;
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
            utilizator = new Client(nume, prenume, nume_utilizator, parola, nr_telefon, data_nasterii, email, adresa_livrare);
        }
        else if (userType == "Bucatar") {
            db.InsertChef(wNume, wPrenume, wNumeUtilizator, wParola, wNrTelefon,
                wDataNasterii, wEmail, experienta, wLinkDemonstrativ);
            std::cout << "Chef registered successfully!" << std::endl;
            utilizator = new Bucatar(nume, prenume, nume_utilizator, parola, nr_telefon, data_nasterii, email,experienta,link_demonstrativ);
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



void DummyChef::handleForgotPassword(const std::string& request) {
    std::istringstream iss(request);
    std::string command, email;
    iss >> command >> email;

    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wEmail = converter.from_bytes(email);

        if (db.UserExists(wEmail)) {
            // Generează și stochează codul de resetare
            std::srand(static_cast<unsigned>(std::time(nullptr)));
            resetCode = 100000 + std::rand() % 900000; // Stocăm codul în variabila unică
            currentEmail = email; // Stocăm email-ul asociat

            std::cout << "[FORGOT_PASSWORD] Cod pentru " << email << ": " << resetCode << std::endl;

            // Trimitem doar EMAIL_FOUND, conform așteptărilor clientului
            std::string response = "EMAIL_FOUND";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "EMAIL_NOT_FOUND";
            send(clientSocket, response.c_str(), response.length(), 0);
        }

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare în handleForgotPassword: " << e.what() << std::endl;
        std::string response = "FORGOT_PASSWORD_ERROR: " + std::string(e.what());
        send(clientSocket, response.c_str(), response.length(), 0);
    }
}


void DummyChef::handleResetPassword(const std::string& request) {
    std::istringstream iss(request);
    std::string command, email, codeStr, newPassword;
    iss >> command >> email >> codeStr >> newPassword;

    try {
        int receivedCode = std::stoi(codeStr);

        // Verificăm dacă codul și email-ul sunt valide
        if (email == currentEmail && receivedCode == resetCode) {
            DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
            db.Connect();

            std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
            std::wstring wEmail = converter.from_bytes(email);
            std::wstring wNewPassword = converter.from_bytes(newPassword);

            // Actualizăm parola în baza de date
            std::wstring sqlQuery = L"UPDATE Utilizatori SET Parola = N'" + wNewPassword + L"' WHERE Email = N'" + wEmail + L"'";
            db.ExecuteNonQuery(sqlQuery);

            // Resetăm codul și email-ul
            resetCode = 0;
            currentEmail = "";

            std::string response = "RESET_SUCCESS";
            send(clientSocket, response.c_str(), response.length(), 0);

            std::cout << "[RESET_PASSWORD] Parola pentru " << email << " a fost resetată cu succes!" << std::endl;

            db.Disconnect();
        }
        else {
            std::string response = "RESET_FAILED";
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare în handleResetPassword: " << e.what() << std::endl;
        std::string response = "RESET_ERROR: " + std::string(e.what());
        send(clientSocket, response.c_str(), response.length(), 0);
    }
}



void DummyChef::handleClientPreferences(const std::string& request) {
    std::istringstream iss(request);
    std::string command, username, foodPreferences, allergies, deliveryTime, pricePreference, notes;
    iss >> command >> username >> std::quoted(foodPreferences) >> std::quoted(allergies) >>
        std::quoted(deliveryTime) >> std::quoted(pricePreference) >> std::quoted(notes);

    // Log the received values for debugging
    std::cout << "Received deliveryTime: " << deliveryTime << std::endl;

    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        // Convert strings to wstrings for database compatibility
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wUsername = converter.from_bytes(username);
        std::wstring wFoodPreferences = converter.from_bytes(foodPreferences);
        std::wstring wAllergies = converter.from_bytes(allergies);
        std::wstring wDeliveryTime = converter.from_bytes(deliveryTime);
        std::wstring wPricePreference = converter.from_bytes(pricePreference);
        std::wstring wNotes = converter.from_bytes(notes);

        // Log the converted deliveryTime for debugging
        std::wcout << L"Converted wDeliveryTime: " << wDeliveryTime << std::endl;

        // Check if user exists
        if (!db.UserExistsByUsername(wUsername)) {
            std::string response = "PREFERINTE_CLIENT_FAILED: User not found";
            send(clientSocket, response.c_str(), response.length(), 0);
            db.Disconnect();
            return;
        }

        // Get client ID
        int clientId = db.GetUserIdByUsername(wUsername);
        if (clientId == -1) {
            std::string response = "PREFERINTE_CLIENT_FAILED: User not found";
            send(clientSocket, response.c_str(), response.length(), 0);
            db.Disconnect();
            return;
        }

        // Check if preferences already exist for this client
        bool preferencesExist = db.PreferencesExist(clientId);

        if (preferencesExist) {
            // Update existing preferences
            db.UpdateClientPreferences(clientId, wFoodPreferences, wAllergies, wDeliveryTime, wPricePreference, wNotes);
        }
        else {
            // Insert new preferences
            db.InsertPreferinte(clientId, wFoodPreferences, wAllergies, wDeliveryTime, wPricePreference, wNotes);
        }

        std::string response = "PreferinteSucces";
        send(clientSocket, response.c_str(), response.length(), 0);
        std::cout << "Client preferences saved for username: " << username << std::endl;

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in handleClientPreferences: " << e.what() << std::endl;
        std::string response = "PREFERINTE_CLIENT_ERROR: " + std::string(e.what());
        send(clientSocket, response.c_str(), response.length(), 0);
    }
}