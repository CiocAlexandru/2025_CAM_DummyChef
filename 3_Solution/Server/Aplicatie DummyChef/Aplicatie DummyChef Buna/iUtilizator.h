#pragma once
#include<iostream>
#include<string>

class iUtilizator
{
public:
	virtual std::string getTypeUser()const = 0;
	virtual void printInfo()const = 0;
};

