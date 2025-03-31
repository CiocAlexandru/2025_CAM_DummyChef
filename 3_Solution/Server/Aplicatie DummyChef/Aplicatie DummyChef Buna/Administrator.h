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
	Administrator() {};
	~Administrator() {};
public:
	static Administrator* getInstance();
	static void destroyInstance();
};

