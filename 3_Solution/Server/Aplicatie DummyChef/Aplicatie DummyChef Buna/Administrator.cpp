#include "Administrator.h"
#include "DatabaseConnection.h"



Administrator* Administrator::instance = nullptr;

Administrator* Administrator::getInstance()
{
    if (instance == nullptr)
        instance = new Administrator();
    return instance;
}

void Administrator::destroyInstance()
{
    if (instance != nullptr)
    {
        delete instance;
        instance = nullptr;
    }
}


#include <fstream>
#include <sstream>

void Administrator::addFurnizori() 
{
    std::ifstream file("furnizori.txt");
    if (!file.is_open()) {
        std::cerr << "Eroare la deschiderea fisierului furnizori.txt" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream linestream(line);
        std::string nume, telefon, email, adresa;

        if (std::getline(linestream, nume, '|') &&
            std::getline(linestream, telefon, '|') &&
            std::getline(linestream, email, '|') &&
            std::getline(linestream, adresa, '|'))
        {
            Furnizor* furnizor = new Furnizor(nume, telefon, email, adresa);
            furnizori.push_back(furnizor);
        }
    }

    file.close();
}



void Administrator::insertFurnizori(std::vector<Furnizor*> furnizoriExterni) 
{
    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    for (Furnizor* furnizor : furnizoriExterni) {
        std::string numeStr = furnizor->GetNume();
        std::string telStr = furnizor->GetTelefon();
        std::string emailStr = furnizor->GetEmail();
        std::string adrStr = furnizor->GetAdresaLivrare();

        std::wstring nume(numeStr.begin(), numeStr.end());
        std::wstring telefon(telStr.begin(), telStr.end());
        std::wstring email(emailStr.begin(), emailStr.end());
        std::wstring adresa(adrStr.begin(), adrStr.end());

        try {
            db.InsertFurnizor(nume, telefon, email, adresa);
            std::wcout << L"Furnizor adaugat: " << nume << std::endl;
        }
        catch (const std::exception& ex) {
            std::cerr << "Eroare la inserarea furnizorului: " << ex.what() << std::endl;
        }
    }

    db.Disconnect();
}

void Administrator::addIngrediente() 
{
    std::ifstream file("ingrediente.txt");
    if (!file.is_open()) {
        std::cerr << "Eroare la deschiderea fisierului ingrediente.txt" << std::endl;
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream linestream(line);
        std::string nume, pretStr, furnizorIdStr;

        if (std::getline(linestream, nume, '|') &&
            std::getline(linestream, pretStr, '|') &&
            std::getline(linestream, furnizorIdStr, '|'))
        {
            try {
                double pret = std::stod(pretStr);
                int idFurnizor = std::stoi(furnizorIdStr);

                Ingrediente* ing = new Ingrediente(nume, pret, idFurnizor);
                ingrediente.push_back(ing);
            }
            catch (...) {
                std::cerr << "Eroare la procesarea liniei: " << line << std::endl;
            }
        }
    }

    file.close();
}


void Administrator::insertIngrediente(std::vector<Ingrediente*> ingrediente) 
{
   
    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    for (Ingrediente* ing : ingrediente) {
        std::string numeStr = ing->GetNume();
        std::wstring nume(numeStr.begin(), numeStr.end());

        double pret = ing->GetPret();
        int furnizorId = ing->GetFurnizorId();

        try {
            db.InsertIngredient(nume, pret, furnizorId);
            std::wcout << L"Ingredient adaugat în DB: " << nume << L" (pret: " << pret << L", furnizor: " << furnizorId << L")" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Eroare la inserarea ingredientului " << numeStr << ": " << e.what() << std::endl;
        }
    }


    db.Disconnect();
}

void Administrator::addStoc() {
    std::ifstream file("Stoc.txt");
    if (!file.is_open()) {
        std::cerr << "Eroare la deschiderea fisierului stoc.txt" << std::endl;
        return;
    }

    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string idStr, cantStr;

        if (std::getline(ss, idStr, '|') && std::getline(ss, cantStr, '|')) {
            try {
                int ingredientId = std::stoi(idStr);
                int cantitate = std::stoi(cantStr);

                db.InsertStock(ingredientId, cantitate);
                std::wcout << L"Stoc adaugat: IngredientID = " << ingredientId << L", Cantitate = " << cantitate << std::endl;
            }
            catch (...) {
                std::cerr << "Eroare la procesarea liniei: " << line << std::endl;
            }
        }
    }

    db.Disconnect();
    file.close();
}

void Administrator::vizualizareServer()
{
    std::ifstream logFile("Log.txt");

    if (!logFile.is_open()) {
        std::cout << "Eroare: Nu s-a putut deschide fisierul Log.txt.\n";
        return;
    }

    std::string line;
    while (std::getline(logFile, line)) {
        std::cout << line << std::endl;
    }

    logFile.close();
}


void Administrator::menu()
{
    std::cout << "Bine ati venit!\n";
    int n;
    do
    {
        std::cout << "Pentru a finaliza mentenanta apasati tasta 0:\n";
        std::cout << "Pentru a introduce furnizorii in baza de date apasati tasta 1:\n";
        std::cout << "Pentru a introduce ingredientele in baza de date apasati tasta 3:\n";
        std::cout << "Pentru a introduce stocul alimentelor in baza de date apasati tasta 5:\n";
        std::cout << "Pentru a vizualiza datele din logger apasati tasta 7:\n";
        std::cout << "Pentru a vizualiza toti furnizorii din baza de date apasati tasta 9:\n";
        std::cout << "Pentru a vizualiza toti utilizatorii(clienti/bucatari) din baza de date apasati 11:\n";
        std::cout << "Pentru a vizualiza toate retetele din baza de date apasati 13:\n";
        std::cout << "Pentru a vizualiza toate comenzile efectuate de toti clientii din baza de date apasati 15:\n";
        std::cout << "Pentru a vizualiza preferintele fiecarui client din baza de date apasati 17:\n";
        std::cout << "Pentru a sterge un utilizator(client/bucatar) din baza de date apasati 19:\n";
        std::cin >> n;
        switch (n)
        {
        case 1:
            this->addFurnizori();
            break;
        case 3:
            this->addIngrediente();
            break;
        case 5:
            this->addStoc();
            break;
        case 7:
            this->vizualizareServer();
            break;
        case 9:
            this->afisareFurnizori();
            break;
        case 11:
            this->afisareUtilizatori();
            break;
        case 13:
            this->afisareRetete();
            break;
        case 15:
            this->afisareComenzi();
            break;
        case 17:
            this->afisarePreferinteClientiCuObiecte();
            break;
        case 19:
            this->stergereUtilizator();
            break;
        }
    } while (n);

}


void Administrator::afisareFurnizori() {
    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    this->furnizori = db.GetAllFurnizori();

    std::cout << "Furnizori existenti in baza de date:\n";
    for (Furnizor* f : this->furnizori) {
        f->printFurnizor(); // metodă deja definită în clasa ta
    }

    // Curățare memorie
    for (Furnizor* f : this->furnizori) {
        delete f;
    }

    db.Disconnect();
}


void Administrator::afisareUtilizatori() {
    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    this->utilizatori = db.GetAllUtilizatori();

    std::cout << "\nUtilizatori existenti in baza de date:\n";
    for (Utilizator* u : utilizatori) {
        u->printInfo();
        std::cout << "\n";
    }

    for (Utilizator* u : utilizatori)
        delete u;

    db.Disconnect();
}


void Administrator::afisareRetete() {
    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    this->retete = db.GetAllRetete();

    std::cout << "\nLista de retete existente in baza de date:\n";
    for (Reteta* r : retete) {
        r->printReteta();
        std::cout << "------------------------------\n";
    }

    for (Reteta* r : retete) {
        for (auto& pair : r->ingredienteNecesare)
            delete pair.second;
        delete r;
    }

    db.Disconnect();
}



void Administrator::afisareComenzi() {
    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    this->comenzi= db.GetAllComenzi();

    std::cout << "\nComenzi efectuate:\n";
    for (Comanda* c : comenzi) {
        c->PrintComanda();
    }

    for (Comanda* c : comenzi) {
        delete c;
    }

    db.Disconnect();
}






void Administrator::afisarePreferinteClientiCuObiecte() {
    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    std::vector<Client*> clienti = db.GetClientiCuPreferinte();

    for (Client* c : clienti) {
        c->printInfo();
        Preferinte* p = c->getPreferinte();
        if (p) {
            p->printPreferinte();
        }
        std::cout << "------------------------\n";
        delete p;
        delete c;
    }

    db.Disconnect();
}



void Administrator::stergereUtilizator() {
    std::string email;
    std::cout << "Introduceti emailul utilizatorului de sters: ";
    std::cin >> email;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
    std::wstring wemail = conv.from_bytes(email);

    DatabaseConnection db(L"DESKTOP-OM4UDQM\\SQLEXPRESS", L"DummyChefDB", L"", L"");
    db.Connect();

    bool success = db.DeleteUserByEmail(wemail);
    db.Disconnect();

    if (success) {
        std::cout << "Utilizatorul cu emailul \"" << email << "\" a fost sters cu succes.\n";
    }
    else {
        std::cout << "Eroare: utilizatorul nu a putut fi sters sau nu exista.\n";
    }
}
