#pragma once
#include "iExceptie.h"
#include <iostream>
#include <string>

class Exceptie:public iExceptie
{
public:
	Exceptie(int cod, std::string eroare) :id(id), eroare(eroare) {};
	friend std::ostream& operator << (std::ostream& out,const Exceptie& exceptie)
	{
		out << exceptie.id << " " << exceptie.eroare << "\n";
		return out;
	};
private:
	int id;
	std::string eroare;
};

