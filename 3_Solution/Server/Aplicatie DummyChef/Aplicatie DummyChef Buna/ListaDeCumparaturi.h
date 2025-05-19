#pragma once
#include<map>
#include "Furnizor.h"
#include "Ingrediente.h"

class ListaDeCumparaturi
{
public:
	std::map<float, Ingrediente*>listaCumparaturi;
	Furnizor* furnizor;
	void printListaDeCumparaturi(){
        std::cout << "Lista contine urmatoarele ingrediente:\n";
        for (const auto& pereche : listaCumparaturi) {
            if (pereche.second) {
                std::cout << "- " << pereche.second->GetNume()
                    << ": " << pereche.first << " lei\n";
            }
            std::cout << "Furnizorul ingredientelor este:\n";
            std::cout << this->furnizor->GetNume() << " " << this->furnizor->GetEmail() << " " << this->furnizor->GetTelefon();
        }
    }
    void addFurnizor(Furnizor* furnizor) { this->furnizor = furnizor; }
    void addIngredient(float pret, Ingrediente* ingredient) { this->listaCumparaturi.insert({ pret, ingredient }); }
};

