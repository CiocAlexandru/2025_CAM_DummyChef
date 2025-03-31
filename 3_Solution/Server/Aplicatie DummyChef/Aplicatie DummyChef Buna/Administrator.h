#pragma once
#include<iostream>
#include<string>
#include<vector>
#include"Utilizator.h"

class Administrator
{
private:
	static Administrator* instance;
	std::vector<Utilizator*>utilizatori;
public:
	static Administrator* getInstance();
};

