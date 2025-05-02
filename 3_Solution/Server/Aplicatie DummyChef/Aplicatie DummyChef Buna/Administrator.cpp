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
            std::wcout << L"Furnizor adãugat: " << nume << std::endl;
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
            std::wcout << L"Ingredient adãugat în DB: " << nume << L" (pret: " << pret << L", furnizor: " << furnizorId << L")" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Eroare la inserarea ingredientului " << numeStr << ": " << e.what() << std::endl;
        }
    }


    db.Disconnect();
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
        std::cin >> n;
        switch (n)
        {
        case 1:
            this->addFurnizori();
            this->insertFurnizori(this->furnizori);
            break;
        case 3:
            this->addIngrediente();
            this->insertIngrediente(this->ingrediente);
            break;
        }
    } while (n);

}
