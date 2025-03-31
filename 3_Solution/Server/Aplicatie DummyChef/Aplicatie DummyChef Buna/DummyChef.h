#pragma once
#include "Utilizator.h"
#include "Ingrediente.h"
#include "Reteta.h"
#include "Furnizor.h"
#include <vector>
#include "Administrator.h"


class DummyChef
{
public:
	static DummyChef* getApp(Administrator* administrator);
	static void destroyApp();
	void menu();
private:
	DummyChef(Administrator* administrator) { this->administrator = administrator; };
	~DummyChef() { Administrator::destroyInstance; };
	std::vector<Utilizator*>utilizatori;
	std::vector<Ingrediente*>ingrediente;
	std::vector<Reteta*>retete;
	std::vector<Furnizor*>furnizori;
	Administrator* administrator;
	static DummyChef* app;

};

