#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma comment(lib, "odbc32.lib")
#pragma comment(lib, "odbccp32.lib")
#include "Utilizator.h"
#include "Ingrediente.h"
#include "Reteta.h"
#include "Furnizor.h"
#include <vector>
#include "Administrator.h"
#include "DatabaseConnection.h"
#include "ExceptieSocket.h"
#include <string>
#include <iostream>



class DummyChef
{
public:
	static DummyChef* getApp(Administrator* administrator);
	static void destroyApp();
	void InsertNewProduct();
	void InsertNewClient();
	void connectToClient();
	void closeSocket();
	void run();
	bool handleLogin(const std::string& email, const std::string& password);
	std::string forgotPassword(const std::string& email);
	void registerUser(const std::string& userType, const std::string& nume, const std::string& prenume,
		const std::string& nume_utilizator, const std::string& parola, const std::string& nr_telefon,
		const std::string& data_nasterii, const std::string& email,
		const std::string& adresa_livrare = "", int experienta=0, const std::string& link_demonstrativ = "");

private:
	DummyChef(Administrator* administrator):serverSocket(INVALID_SOCKET),clientSocket(INVALID_SOCKET) 
	{ this->administrator = administrator; };
	~DummyChef() { Administrator::destroyInstance(); closeSocket();};
	std::vector<Ingrediente*>ingrediente;
	Reteta* retete;
	Utilizator* utilizator;
	std::vector<Furnizor*>furnizori;
	Administrator* administrator;
	static DummyChef* app;
	SOCKET serverSocket;
	SOCKET clientSocket;

};

