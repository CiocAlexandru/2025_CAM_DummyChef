#pragma once

#include "Exceptie.h"

class ExceptieSocket:public Exceptie
{
public:
	ExceptieSocket(int cod, std::string eroare, int linie) :Exceptie(cod, eroare) {};
	virtual void printInfo()override {
		std::cout << this->id << ". " << this->eroare <<"  "<< linie<<"\n";
	}
private:
	int linie;
};

