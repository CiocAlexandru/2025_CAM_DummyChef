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
#include <string>
#include <iostream>
#include "Logger.h"
#include "IDGenerator.h"


class DummyChef
{
public:
	static DummyChef* getApp(Administrator* administrator);
	static void destroyApp();
	void connectToClient();
	void closeSocket();
	void run();
	void handleClient();
	bool handleLogin(const std::string& email, const std::string& password);
	void handleForgotPassword(const std::string& request);
	void handleResetPassword(const std::string& request);
	void registerUser(const std::string& userType, const std::string& nume, const std::string& prenume,
		const std::string& nume_utilizator, const std::string& parola, const std::string& nr_telefon,
		const std::string& data_nasterii, const std::string& email,
		const std::string& adresa_livrare = "", int experienta = 0, const std::string& link_demonstrativ = "");
	void handleClientPreferences(const std::string& request);
	void handleAddRecipeByClient(const std::string& request);
	void handleAddIngredientByClient(const std::string& request);
	void handleGetMyRecipes(const std::string& request);
	void handleSearchRecipes(const std::string& request);
	void handleGenerareLista(const std::string& request);
	void handlePlaceOrder(const std::string& request);
	void handleGetOrders(const std::string& request);

private:
	DummyChef(Administrator* administrator) :serverSocket(INVALID_SOCKET), clientSocket(INVALID_SOCKET), resetCode(0),
		currentEmail("") {
		this->administrator = administrator;
		this->log = new Logger("Log.txt");
		this->idGenerator = new IDGenerator();
	};
	~DummyChef() { Administrator::destroyInstance(); closeSocket(); };
	std::vector<Ingrediente*>ingrediente;
	Reteta* retete;
	Utilizator* utilizator;
	Administrator* administrator;
	static DummyChef* app;
	int resetCode;
	std::string currentEmail;
	SOCKET serverSocket;
	SOCKET clientSocket;
	Logger* log;
	IDGenerator* idGenerator;
};

