#pragma once
#include "Utilizator.h"
#include<iostream>
#include<string>

class Bucatar : public Utilizator
{
private:
	int ani_vechime;
	std::string link_demonstrativ;
public:
	Bucatar(std::string nume, std::string prenume, std::string nume_utilizator, std::string parola, std::string nr_telefon,
		std::string data_nasterii, std::string email,int ani_vechime,std::string link_doveditor):
		Utilizator(nume,  prenume, nume_utilizator, parola,  nr_telefon, data_nasterii, email),ani_vechime(ani_vechime),link_demonstrativ(link_demonstrativ){ }
	std::string getTypeUser()const override
	{
		return "Bucatar";
	}
	void printInfo()const override;
};

