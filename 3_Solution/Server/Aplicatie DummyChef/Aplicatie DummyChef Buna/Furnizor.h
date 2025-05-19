#pragma once
#include "iFurnizior.h"
#include <string>

class Furnizor : public iFurnizior {
private:
    std::string nume;
    std::string telefon;
    std::string email;
    std::string adresaLivrare;

public:
    Furnizor(const std::string& nume,
        const std::string& telefon,
        const std::string& email,
        const std::string& adresaLivrare)
        : nume(nume), telefon(telefon), email(email), adresaLivrare(adresaLivrare) {
    }


    std::string GetNume() const  { return nume; }
    std::string GetTelefon() const  { return telefon; }
    std::string GetEmail() const  { return email; }
    std::string GetAdresaLivrare() const  { return adresaLivrare; }
    void printFurnizor()override { std::cout << this->nume << " " << this->email << std::endl; };
    void SetNume(const std::string& name) { nume = name; }
    void SetTelefon(const std::string& phone) { telefon = phone; }
    void SetEmail(const std::string& mail) { email = mail; }
    void SetAdresaLivrare(const std::string& adress) { adresaLivrare = adress; }
};
