#pragma once
#include <iostream>


class Preferinte
{
public:
	Preferinte(std::string preferinteAlimentare, std::string alergii, std::string oraLivrare, std::string preferintaPret,
		std::string notite) :preferinteAlimentare(preferinteAlimentare), alergii(alergii), oraLivrare(oraLivrare),
		preferintaPret(preferintaPret), notite(notite) {};
	std::string getPreferinteAlimentare() { return preferinteAlimentare; };
	std::string getAlergii() { return alergii; };
	std::string getOraLivrare() { return oraLivrare; };
	std::string getPreferintePret() { return preferintaPret; };
	std::string getNotite() { return notite; };
	void printPreferinte() {
		std::cout << "Preferinte alimentare: " << preferinteAlimentare << "\n";
		std::cout << "Alergii: " << alergii << "\n";
		std::cout << "Ora livrare: " << oraLivrare << "\n";
		std::cout << "Preferinta de pret: " << preferintaPret << "\n";
		std::cout << "Notite suplimentare: " << notite << "\n";
	}
private:
	std::string preferinteAlimentare;
	std::string alergii;
	std::string oraLivrare;
	std::string preferintaPret;
	std::string notite;

};

