#pragma once
#include "iExceptie.h"
#include <string>

class Exceptie:public iExceptie
{
public:
	Exceptie(int cod, std::string eroare) :id(id), eroare(eroare) {};
	virtual void printInfo() override {
		std::cout << this->id << ". " << this->eroare << "\n";
	}
protected:
	int id;
	std::string eroare;
};

