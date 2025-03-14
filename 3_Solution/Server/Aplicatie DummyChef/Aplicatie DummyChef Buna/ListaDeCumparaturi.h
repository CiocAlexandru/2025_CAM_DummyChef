#pragma once
#include<map>
#include "Furnizor.h"
#include "Ingrediente.h"

class ListaDeCumparaturi
{
public:
	std::map<float, Ingrediente*>listaCumparaturi;
	Furnizor* furnizor;
};

