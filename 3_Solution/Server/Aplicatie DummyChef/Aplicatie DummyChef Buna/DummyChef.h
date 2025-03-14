#pragma once
#include "Utilizator.h"
#include "Ingrediente.h"
#include "Reteta.h"
#include "Furnizor.h"
#include <vector>


class DummyChef
{
private:
	std::vector<Utilizator*>utilizatori;
	std::vector<Ingrediente*>ingrediente;
	std::vector<Reteta*>retete;
	std::vector<Furnizor*>furnizori;
};

