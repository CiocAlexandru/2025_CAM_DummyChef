#pragma once
#include "iReteta.h"
#include "Ingrediente.h"
#include <map>

class Reteta :public iReteta
{
public:
	std::map<int, Ingrediente*>ingredienteNecesare;
};

