#pragma once
#include "iReteta.h"
#include "Ingrediente.h"
#include <map>
#include <string>
#include <iostream>

class Reteta : public iReteta
{
private:
    std::string denumire;
    std::string timpPreparare;
    std::string pasiPreparare;

public:
    std::map<int, Ingrediente*> ingredienteNecesare;

    Reteta() = default;

    Reteta(const std::string& den, const std::string& timp, const std::string& pasi)
        : denumire(den), timpPreparare(timp), pasiPreparare(pasi) {
    }

    void setDenumire(const std::string& d) { denumire = d; }
    void setTimpPreparare(const std::string& t) { timpPreparare = t; }
    void setPasiPreparare(const std::string& p) { pasiPreparare = p; }

    void getType() override {
        std::cout << "Reteta normala";
    }

    void printReteta() override {
        std::cout << "\n=== Reteta: " << denumire << " ===\n";
        std::cout << "Timp de preparare: " << timpPreparare << "\n";
        std::cout << "Pasi de preparare: " << pasiPreparare << "\n";
        std::cout << "Ingrediente necesare:\n";

        for (const auto& pereche : ingredienteNecesare) {
            if (pereche.second) {
                std::cout << "- " << pereche.second->GetNume()
                    << ": " << pereche.first << " unitati\n";
            }
        }
    }

    void addIngredient(int cantitate, Ingrediente* ingredient) {
        this->ingredienteNecesare.insert({ cantitate, ingredient });
    }
};


