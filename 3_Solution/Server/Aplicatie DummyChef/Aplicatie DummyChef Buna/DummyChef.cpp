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
        std::cout << "Bun venit in serverul aplicatiei DummyChef!\n";
        
        int n;
        do {
            std::cout << "Pentru inchiderea aplicatiei apsati tasta 0:\n";
            std::cout << "Pentru pornirea serverului apsati tasta 1:\n";
            std::cout << "Pentru mentenanta (ocupati functia de administrator) apsati tasta 5:\n";
            std::cin >> n;
            switch (n)
            {
                
            case 1:
                connectToClient();
                closeSocket();
                break;
            case 5:
                administrator->menu();
                break;
            }
        } while (n);
    }
    catch (...) {
        closeSocket(); // Asigura-te că socket-urile sunt inchise in caz de eroare
        this->log->add("Eroare!\n");
        throw; // Re-arunca exceptia pentru gestionare ulterioara
    }
    
}


void DummyChef::connectToClient()
{

    WSADATA wsaData;
    int wsaInitResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (wsaInitResult != 0) {
        std::cerr << "Eroare Winsock: " << wsaInitResult << std::endl;
        std::string mesaj = "Eroare Winsock: \n";
        return;
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET) {
        WSACleanup();
        std::cerr << "Eroare la crearea socketului server." << std::endl;
        std::string mesaj = "Eroare la crearea socketului server.\n";
        log->add(mesaj);
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
        log->add("Eroare la bind!\n");
        return;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR) {
        closesocket(serverSocket);
        WSACleanup();
        std::cerr << "Eroare la listen." << std::endl;
        log->add("Eroare la listen!\n");
        return;
    }

    std::cout << "Serverul asteapta conexiuni...\n";

    while (true) {
        sockaddr_in clientAddr;
        int clientAddrSize = sizeof(clientAddr);
        clientSocket = accept(serverSocket, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrSize);

        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Eroare la acceptarea conexiunii.\n";
            log->add("Eroare la acceptarea conexiunii!\n");
            continue;
        }

        log->add("Client conectat!\n");
        std::cout << "Client conectat!\n";

        handleClient(); 

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
            log->add("Eroare la receptionare.Inchidem clientul\n");
            break;
        }

        if (bytesReceived == 0) {
            std::cout << "Clientul s-a deconectat.\n";
            log->add("Clientul s-a deconectat.\n");
            break;
        }

        std::string receivedMessage(buffer);
        std::cout << "Client: " << receivedMessage << std::endl;
        std::string mesaj= "Client" + receivedMessage + "\n";
        log->add(mesaj);
        try {
            if (receivedMessage.rfind("LOGIN ", 0) == 0) {
                std::istringstream iss(receivedMessage);
                std::string command, email, password;
                iss >> command >> email >> password;

                if (handleLogin(email, password)) {
                    std::string response = "LOGIN_SUCCESS";
                    log->add(response + "\n");
                    send(clientSocket, response.c_str(), response.length(), 0);
                }
                else {
                    std::string response = "LOGIN_FAILED";
                    log->add(response+"\n");
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
                    log->add(response+"\n");
                    send(clientSocket, response.c_str(), response.size(), 0);
                }
                else if (userType == "CHEF") {
                    std::string nume, prenume, username, parola, telefon, data_nasterii, email, link_demo;
                    int experienta;
                    iss >> nume >> prenume >> username >> parola >> telefon >> data_nasterii >> email >> experienta >> link_demo;

                    registerUser("Bucatar", nume, prenume, username, parola, telefon,
                        data_nasterii, email, "", experienta, link_demo);
                    std::string response = "SIGNUP_CHEF_SUCCESS";
                    log->add(response+"\n");
                    send(clientSocket, response.c_str(), response.size(), 0);
                }
                else {
                    send(clientSocket, "REGISTER_INVALID_TYPE", 22, 0);
                    log->add("REGISTER_INVALID_TYPE\n");
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
            else if (receivedMessage.rfind("ADD_RECIPE|", 0) == 0) 
            {
                handleAddRecipeByClient(receivedMessage);
            }
            else if (receivedMessage.rfind("ADD_INGREDIENT|", 0) == 0) 
            {
                handleAddIngredientByClient(receivedMessage);
            }
            else if (receivedMessage.rfind("GET_MY_RECIPES|", 0) == 0)
            {
                handleGetMyRecipes(receivedMessage);
            }
            else if (receivedMessage.rfind("SEARCH_RECIPES ", 0) == 0) 
            {
                handleSearchRecipes(receivedMessage);
            }
            else if (receivedMessage.rfind("GENERARE_LISTA ", 0) == 0) {
                handleGenerareLista(receivedMessage);
            }
            else {
                send(clientSocket, "UNKNOWN_COMMAND", 16, 0);
                log->add("UNKNOWN_COMMAND\n");
            }
           
            
        }
        catch (const std::exception& e) {
            std::string error = "SERVER_ERROR: " + std::string(e.what());
            log->add(error+"\n");
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
            std::string mesaj = "Login successful! Welcome " + this->utilizator->getNume() + "\n";
            log->add(mesaj);

            // Determină tipul utilizatorului prin dynamic_cast
            std::string response;
            if (dynamic_cast<Client*>(this->utilizator)) {
                response = "LOGIN_SUCCESS_CLIENT";
                log->add(response + "\n");
            }
            else if (dynamic_cast<Bucatar*>(this->utilizator)) {
                response = "LOGIN_SUCCESS_CHEF";
                log->add(response + "\n");
            }
            else {
                response = "LOGIN_FAILED";
                log->add(response + "\n");
            }

            send(clientSocket, response.c_str(), response.length(), 0);
            db.Disconnect();
            return true;
        }
        else {
            std::cout << "Login failed! Invalid credentials." << std::endl;
            std::string response = "LOGIN_FAILED";
            log->add(response + "\n");
            send(clientSocket, response.c_str(), response.length(), 0);
            db.Disconnect();
            return false;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error during login: " << e.what() << std::endl;
        log->add("Error during login\n");
        return false;
    }
}

void DummyChef::registerUser(const std::string& userType, const std::string& nume, const std::string& prenume,
    const std::string& nume_utilizator, const std::string& parola, const std::string& nr_telefon,
    const std::string& data_nasterii, const std::string& email,
    const std::string& adresa_livrare , int experienta, const std::string& link_demonstrativ )
{
    try {

        if (userType=="Bucatar")
        {
            std::string confirmInput;
            std::cout << "Confirmati inregistrarea (scrieți OK daca link-ul demonstrativ este valabil pentru a continua), daca nu False: ";
            std::cin >> confirmInput;

            if (confirmInput != "OK") {
                std::cerr << "Eroare: Inregistrarea a fost anulata de utilizator." << std::endl;
                log->add("Eroare: Inregistrarea a fost anulata de utilizator.\n");
                std::string response = "Link_demonstrativ_refuzat";
                send(clientSocket, response.c_str(), response.size(), 0);
                return;
            }
        }
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
            log->add("Error: A user with this email already exists!\n");
            db.Disconnect();
            return;
        }

        if (userType == "Client") {
            db.InsertClient(wNume, wPrenume, wNumeUtilizator, wParola, wNrTelefon,
                wDataNasterii, wEmail, wAdresaLivrare);
            std::cout << "Client registered successfully!" << std::endl;
            log->add("Client registered successfully!\n");
            utilizator = new Client(nume, prenume, nume_utilizator, parola, nr_telefon, data_nasterii, email, adresa_livrare);
        }
        else if (userType == "Bucatar") {
            db.InsertChef(wNume, wPrenume, wNumeUtilizator, wParola, wNrTelefon,
                wDataNasterii, wEmail, experienta, wLinkDemonstrativ);
            std::cout << "Chef registered successfully!" << std::endl;
            log->add("Chef registered successfully!\n");
            utilizator = new Bucatar(nume, prenume, nume_utilizator, parola, nr_telefon, data_nasterii, email,experienta,link_demonstrativ);
        }
        else {
            std::cout << "Error: Invalid user type specified!" << std::endl;
            log->add("Error: Invalid user type specified!\n");
        }

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error during registration: " << e.what() << std::endl;
        log->add("Error during registration\n");
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
            log->add("EMAIL_FOUND\n");
            send(clientSocket, response.c_str(), response.length(), 0);
        }
        else {
            std::string response = "EMAIL_NOT_FOUND";
            log->add("EMAIL_NOT_FOUND\n");
            send(clientSocket, response.c_str(), response.length(), 0);
        }

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare în handleForgotPassword: " << e.what() << std::endl;
        std::string response = "FORGOT_PASSWORD_ERROR: " + std::string(e.what());
        log->add("FORGOT_PASSWORD_ERROR:\n");
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
            log->add("[RESET_PASSWORD] Parola pentru " + email + " a fost resetată cu succes!\n");
            db.Disconnect();
        }
        else {
            std::string response = "RESET_FAILED";
            log->add(response + "\n");
            send(clientSocket, response.c_str(), response.length(), 0);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Eroare în handleResetPassword: " << e.what() << std::endl;
        std::string response = "RESET_ERROR: " + std::string(e.what());
        log->add(response + "\n");
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
            log->add(response + "\n");
            send(clientSocket, response.c_str(), response.length(), 0);
            db.Disconnect();
            return;
        }

        // Get client ID
        int clientId = db.GetUserIdByUsername(wUsername);
        if (clientId == -1) {
            std::string response = "PREFERINTE_CLIENT_FAILED: User not found";
            send(clientSocket, response.c_str(), response.length(), 0);
            log->add(response + "\n");
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
        int bytesSent = send(clientSocket, response.c_str(), response.length(), 0);
        std::cout << "Trimis catre client: " << response << " (" << bytesSent << " bytes)" << std::endl;
        std::cout << "Client preferences saved for username: " << username << std::endl;
        log->add("Client preferences saved for username: " + username + "\n");

        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::cerr << "Error in handleClientPreferences: " << e.what() << std::endl;
        std::string response = "PREFERINTE_CLIENT_ERROR: " + std::string(e.what());
        send(clientSocket, response.c_str(), response.length(), 0);
        log->add(response + "\n");
    }
}

void DummyChef::handleAddRecipeByClient(const std::string& request) {
    try {
        // Format: ADD_RECIPE_CLIENT|email|nume|timp|ingred1:qty1;ingred2:qty2|pasi
        std::vector<std::string> tokens;
        std::stringstream ss(request);
        std::string token;

        while (std::getline(ss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() != 6) {
            std::string response = "ADD_RECIPE_CLIENT_FAILED: Invalid format";
            send(clientSocket, response.c_str(), response.length(), 0);
            log->add(response + "\n");
            return;
        }

        std::string email = tokens[1];
        std::string numeReteta = tokens[2];
        std::string timpPreparare = tokens[3];
        std::string ingredienteRaw = tokens[4];
        std::string pasiPreparare = tokens[5];

        // Conectare DB
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();


        

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        int userId = db.GetUserIdByEmail(converter.from_bytes(email));
        if (userId == -1) {
            std::string response = "ADD_RECIPE_FAILED: Email not found";
            send(clientSocket, response.c_str(), response.length(), 0);
            log->add(response + "\n");
            db.Disconnect();
            return;
        }


        std::stringstream ingSS(ingredienteRaw);
        std::string ingPair;
        while (std::getline(ingSS, ingPair, ';')) {
            size_t delim = ingPair.find(':');
            if (delim != std::string::npos) {
                std::string nume = ingPair.substr(0, delim);
                std::wstring wNume = converter.from_bytes(nume);

                if (!db.IngredientExists(wNume)) {
                    std::string response = "ADD_RECIPE_FAILED: Ingredient inexistent - " + nume;
                    send(clientSocket, response.c_str(), response.length(), 0);
                    log->add(response + "\n");
                    db.Disconnect();
                    return;
                }
            }
        }


        int idReteta = db.InsertRecipeFromClient(
            converter.from_bytes(numeReteta),
            converter.from_bytes(timpPreparare),
            converter.from_bytes(pasiPreparare),
            userId
        );

        // Ingrediente separate prin ;
        while (std::getline(ingSS, ingPair, ';')) {
            size_t delim = ingPair.find(':');
            if (delim != std::string::npos) {
                std::string nume = ingPair.substr(0, delim);
                std::string cantitate = ingPair.substr(delim + 1);
                db.InsertRecipeIngredient(
                    idReteta,
                    converter.from_bytes(nume),
                    converter.from_bytes(cantitate)
                );
            }
        }

        db.Disconnect();

        std::string response = "ADD_RECIPE_SUCCESS";
        send(clientSocket, response.c_str(), response.length(), 0);
        log->add(response + "\n");
    }
    catch (const std::exception& e) {
        std::string error = "ADD_RECIPE_ERROR: " + std::string(e.what());
        send(clientSocket, error.c_str(), error.length(), 0);
        log->add(error + "\n");
    }
}


void DummyChef::handleAddIngredientByClient(const std::string& request) {
    try {
        // Format: ADD_INGREDIENT|email|nume|pret|stoc|furnizor|telefon|email_furnizor|adresa
        std::vector<std::string> tokens;
        std::stringstream ss(request);
        std::string token;

        while (std::getline(ss, token, '|')) {
            tokens.push_back(token);
        }

        if (tokens.size() != 9) {
            std::string response = "ADD_INGREDIENT_FAILED: Invalid format";
            send(clientSocket, response.c_str(), response.length(), 0);
            log->add(response + "\n");
            return;
        }

        std::string email = tokens[1];
        std::string numeIngredient = tokens[2];
        std::string pretStr = tokens[3];
        std::string cantitateStr = tokens[4];
        std::string numeFurnizor = tokens[5];
        std::string telefon = tokens[6];
        std::string emailFurnizor = tokens[7];
        std::string adresa = tokens[8];

        double pret = std::stod(pretStr);
        int cantitate = std::stoi(cantitateStr);

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        // Verifică dacă furnizorul există
        int furnizorId = db.GetFurnizorIdByEmail(converter.from_bytes(emailFurnizor));
        if (furnizorId == -1) {
            // Inserare furnizor nou
            furnizorId = db.InsertFurnizor(
                converter.from_bytes(numeFurnizor),
                converter.from_bytes(telefon),
                converter.from_bytes(emailFurnizor),
                converter.from_bytes(adresa)
            );
        }

        // Adaugă ingredient
        int ingredientId = db.InsertIngredient(
            converter.from_bytes(numeIngredient),
            pret,
            furnizorId
        );

        // Adaugă în stoc
        db.InsertStock(ingredientId, cantitate);

        db.Disconnect();

        std::string response = "ADD_INGREDIENT_SUCCESS";
        send(clientSocket, response.c_str(), response.length(), 0);
        log->add(response + "\n");
    }
    catch (const std::exception& e) {
        std::string error = "ADD_INGREDIENT_ERROR: " + std::string(e.what());
        send(clientSocket, error.c_str(), error.length(), 0);
        log->add(error + "\n");
    }
}


void DummyChef::handleGetMyRecipes(const std::string& request) {
    try {
        // Format: GET_MY_RECIPES|email
        std::string email = request.substr(strlen("GET_MY_RECIPES|"));

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring wEmail = converter.from_bytes(email);

        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        int userId = db.GetUserIdByEmail(wEmail);
        if (userId == -1) {
            std::string response = "MY_RECIPES|";
            send(clientSocket, response.c_str(), response.length(), 0);
            log->add(response + "\n");
            db.Disconnect();
            return;
        }

        std::vector<std::wstring> recipes = db.GetRecipesByChefId(userId);
        std::string response = "MY_RECIPES|";
        

        for (size_t i = 0; i < recipes.size(); ++i) {
            response += converter.to_bytes(recipes[i]);
            if (i < recipes.size() - 1)
                response += "##";
        }

        send(clientSocket, response.c_str(), response.length(), 0);
        log->add(response + "\n");
        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::string response = "MY_RECIPES_ERROR: ";
        response += e.what();
        log->add(response + "\n");
        send(clientSocket, response.c_str(), response.length(), 0);
    }
}


void DummyChef::handleSearchRecipes(const std::string& request) {
    try {
        std::string keywordsPart = request.substr(strlen("SEARCH_RECIPES "));
        std::vector<std::string> keywords;
        std::stringstream ss(keywordsPart);
        std::string token;
        while (std::getline(ss, token, ',')) {
            std::transform(token.begin(), token.end(), token.begin(), ::tolower);  // lowercase
            keywords.push_back(token);
        }

        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::wstring query;

        if (keywords.empty() || (keywords.size() == 1 && keywords[0].empty())) {
            query = L"SELECT TOP 10 R.ID, R.Denumire, R.TimpPreparare, R.PasiPreparare "
                L"FROM Retete R ORDER BY R.ID DESC";
        }
        else {
            query = L"SELECT R.ID, R.Denumire, R.TimpPreparare, R.PasiPreparare FROM Retete R WHERE ";
            for (size_t i = 0; i < keywords.size(); ++i) {
                std::wstring kw = converter.from_bytes(keywords[i]);
                query += L"LOWER(R.Denumire) LIKE N'%" + kw + L"%'";
                if (i < keywords.size() - 1)
                    query += L" AND ";
            }
        }

        auto recipes = db.ExecuteQuery(query);
        std::string response = "RECIPE_RESULTS|";

        for (const auto& row : recipes) {
            std::wstring retetaId = row[0];
            std::wstring denumire = row[1];
            std::wstring timp = row[2];
            std::wstring pasi = row[3];

            int recipeId = std::stoi(retetaId);
            auto ingrediente = db.ExecuteQuery(L"SELECT I.Nume, RI.Cantitate "
                L"FROM ReteteIngrediente RI "
                L"JOIN Ingrediente I ON I.ID = RI.IngredientID "
                L"WHERE RI.RetetaID = " + retetaId);

            std::string ingrLine;
            for (const auto& ing : ingrediente) {
                ingrLine += converter.to_bytes(ing[0]) + ":" + converter.to_bytes(ing[1]) + ";";
            }
            if (!ingrLine.empty()) ingrLine.pop_back(); // remove last semicolon

            response += converter.to_bytes(denumire) + "|" +
                converter.to_bytes(timp) + "|" +
                ingrLine + "|" +
                converter.to_bytes(pasi) + "##";
        }

        if (response.back() == '#' && response[response.size() - 2] == '#') {
            response.pop_back();
            response.pop_back();
        }

        send(clientSocket, response.c_str(), response.length(), 0);
        log->add(response + "\n");
        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::string err = "RECIPE_SEARCH_ERROR: " + std::string(e.what());
        send(clientSocket, err.c_str(), err.length(), 0);
        log->add(err + "\n");
    }
}


void DummyChef::handleGenerareLista(const std::string& request) {
    std::istringstream iss(request);
    std::string command, username, recipeName;
    iss >> command >> username;
    std::getline(iss, recipeName);
    recipeName = recipeName.substr(1); // remove leading space

    try {
        DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
        db.Connect();
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;

        // Obține ID client
        int clientId = db.GetUserIdByUsername(converter.from_bytes(username));
        if (clientId == -1) {
            send(clientSocket, "NU_EXISTA_PRODUSE", 17, 0);
            log->add("NU_EXISTA_PRODUSE\n");
            db.Disconnect();
            return;
        }

        // Obține preferințe client
        auto pref = db.ExecuteQuery(L"SELECT Alergii, PreferintaPret FROM PreferinteClienti WHERE IDClient = " + std::to_wstring(clientId));
        if (pref.empty()) {
            send(clientSocket, "NU_EXISTA_PRODUSE", 17, 0);
            log->add("NU_EXISTA_PRODUSE\n");
            db.Disconnect();
            return;
        }

        std::wstring alergii = pref[0][0];
        std::wstring preferintaPret = pref[0][1];

        // Obține ID rețetă
        auto recipeRes = db.ExecuteQuery(L"SELECT ID FROM Retete WHERE Denumire = N'" + converter.from_bytes(recipeName) + L"'");
        if (recipeRes.empty()) {
            send(clientSocket, "NU_EXISTA_PRODUSE", 17, 0);
            log->add("NU_EXISTA_PRODUSE\n");
            db.Disconnect();
            return;
        }

        int recipeId = std::stoi(recipeRes[0][0]);

        // Obține ingredientele rețetei
        auto ingrediente = db.ExecuteQuery(L"SELECT Ingrediente.Nume, Cantitate, Ingrediente.Pret FROM ReteteIngrediente "
            L"JOIN Ingrediente ON ReteteIngrediente.IngredientID = Ingrediente.ID "
            L"WHERE RetetaID = " + std::to_wstring(recipeId));

        double totalPret = 0;
        std::wstringstream listaFinala;

        bool alergic = false;
        for (auto& ing : ingrediente) {
            if (alergii.find(ing[0]) != std::wstring::npos) {
                alergic = true;
                break;
            }
            double pret = std::stod(ing[2]);
            totalPret += pret;
            listaFinala << ing[0] << L" (" << ing[1] << L") - " << pret << L" RON\n";
        }

        if (alergic) {
            send(clientSocket, "NU_EXISTA_PRODUSE", 17, 0);
            log->add("NU_EXISTA_PRODUSE\n");
            db.Disconnect();
            return;
        }

        // Verificare preferințe preț
        bool pretValid = false;
        if (preferintaPret == L"Economic" && totalPret <= 100.0) pretValid = true;
        else if (preferintaPret == L"Moderat" && totalPret > 100.0 && totalPret <= 250.0) pretValid = true;
        else if (preferintaPret == L"Premium" && totalPret > 250.0) pretValid = true;

        if (!pretValid) {
            send(clientSocket, "NU_EXISTA_PRODUSE", 17, 0);
            log->add("NU_EXISTA_PRODUSE\n");
            db.Disconnect();
            return;
        }

        // Obține furnizor
        auto furnizor = db.ExecuteQuery(L"SELECT TOP 1 Nume FROM Furnizori");
        std::wstring numeFurnizor = furnizor.empty() ? L"Furnizor Standard" : furnizor[0][0];

        std::wstringstream mesajClient;
        mesajClient << L"Lista cumpărături pentru rețeta " << converter.from_bytes(recipeName) << L":\n\n";
        mesajClient << listaFinala.str() << L"\n";
        mesajClient << L"Total: " << totalPret << L" RON\n";
        mesajClient << L"Furnizor: " << numeFurnizor;

        std::string response = converter.to_bytes(mesajClient.str());
        send(clientSocket, response.c_str(), response.length(), 0);
        log->add(response+"\n");
        db.Disconnect();
    }
    catch (const std::exception& e) {
        std::string error = "SERVER_ERROR: " + std::string(e.what());
        log->add(error + "\n");
        send(clientSocket, error.c_str(), error.length(), 0);
    }
}
