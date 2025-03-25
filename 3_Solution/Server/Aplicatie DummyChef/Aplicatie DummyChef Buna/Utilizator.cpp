#include "Utilizator.h"

Utilizator::Utilizator(std::string nume, std::string prenume, std::string nume_utilizator, std::string parola, std::string nr_telefon, std::string data_nasterii, std::string email)
{
	this->nume = nume;
	this->prenume = prenume;
	this->nume_utilizator = nume_utilizator;
	this->parola = parola;
	this->nr_telefon = nr_telefon;
	this->data_nasterii = data_nasterii;
	this->email = email;
}

void Utilizator::printInfo() const
{
	std::cout << "Nume: " << nume << std::endl;
	std::cout << "Prenume: " << prenume << std::endl;

}
