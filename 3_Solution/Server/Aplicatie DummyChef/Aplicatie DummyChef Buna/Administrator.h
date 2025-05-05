#pragma once
#include<iostream>
#include<string>
#include<vector>
#include"Utilizator.h"
#include "Furnizor.h"
#include "Ingrediente.h"

class Administrator
{
private:
	static Administrator* instance;
	std::vector<Utilizator*>utilizatori;
	std::vector<Furnizor*>furnizori;
	std::vector<Ingrediente*>ingrediente;
	Administrator() {};
	~Administrator() {};
public:
	static Administrator* getInstance();
	static void destroyInstance();
	void addFurnizori();
	void insertFurnizori(std::vector<Furnizor*>furnizori);
	void addIngrediente();
	void insertIngrediente(std::vector<Ingrediente*>ingrediente);
	void addStoc();
	void vizualizareServer();
	void menu();
};

