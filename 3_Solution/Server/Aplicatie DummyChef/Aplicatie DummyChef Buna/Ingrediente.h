#pragma once
#include "iIngrediente.h"
#include <string>

class Ingrediente:public iIngrediente {
private:
    std::string nume;
    double pret;
    int idFurnizor;

public:
    Ingrediente(const std::string& nume, double pret, int idFurnizor)
        : nume(nume), pret(pret), idFurnizor(idFurnizor) {
    }

    std::string GetNume() const { return nume; }
    double GetPret() const { return pret; }
    int GetFurnizorId() const { return idFurnizor; }

    // Setters (opțional)
    void SetNume(const std::string& name) {nume = name; }
    void SetPret(double price) { pret = price; }
    void SetFurnizorId(int id) { idFurnizor = id; }
};


