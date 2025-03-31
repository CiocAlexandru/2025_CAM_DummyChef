#include "Client.h"

void Client::printInfo() const
{
	std::cout << " \n Informatii utile despre client: \n";
	Utilizator::printInfo();
	std::cout << "Adresa de livrare a clientului: " << adresa_livrare << std::endl;
}
