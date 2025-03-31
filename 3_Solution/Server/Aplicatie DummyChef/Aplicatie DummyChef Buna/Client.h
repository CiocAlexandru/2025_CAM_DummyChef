#pragma once
#include "Utilizator.h"
#include "Preferinte.h"
#include "ListaDeCumparaturi.h"


class Client : public Utilizator
{
private:
	Preferinte* preferinte;
	ListaDeCumparaturi* lista;
	std::string adresa_livrare;
public:
	Client(std::string nume, std::string prenume, std::string nume_utilizator, std::string parola, std::string nr_telefon,
		std::string data_nasterii, std::string email, std::string adresa_livrare) :Utilizator(nume, prenume, nume_utilizator, parola, nr_telefon, data_nasterii, email), adresa_livrare(adresa_livrare) {  }
	std::string getTypeUser()const override
	{
		return "Client";
	}
	void printInfo()const override;
};

