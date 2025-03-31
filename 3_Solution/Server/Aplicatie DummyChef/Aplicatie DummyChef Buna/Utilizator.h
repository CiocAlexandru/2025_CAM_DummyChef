#pragma once
#include "iUtilizator.h"
#include "IDGenerator.h"
#include<iostream>
#include<string>

class Utilizator :public iUtilizator
{
protected:
	IDGenerator* id_unic;
	std::string nume;
	std::string prenume;
	std::string nume_utilizator;
	std::string parola;
	std::string nr_telefon;
	std::string data_nasterii;
	std::string email;
public:
	Utilizator(std::string nume, std::string prenume, std::string nume_utilizator,
		std::string parola, std::string nr_telefon, std::string data_nasterii, std::string email);
	void printInfo()const override;
};

