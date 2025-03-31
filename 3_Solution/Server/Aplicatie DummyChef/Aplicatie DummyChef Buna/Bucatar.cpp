#include "Bucatar.h"

void Bucatar::printInfo() const
{
	std::cout << " \n Informatii utile despre bucatar: \n";
	Utilizator::printInfo();
	std::cout << "Numar ani de vechime in domeniul bucatariei: " << ani_vechime << std::endl;
	std::cout << "Link demonstrativ experienta culinara: " << link_demonstrativ << std::endl;
}
