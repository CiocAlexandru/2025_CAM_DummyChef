#pragma once
#include "Utilizator.h"
#include "Preferinte.h"
#include "ListaDeCumparaturi.h"


class Client : public Utilizator
{
public:
	Preferinte* preferinte;
	ListaDeCumparaturi* lista;
};

