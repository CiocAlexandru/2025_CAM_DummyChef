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
	void RunDatabaseTest();
	void InsertNewClient();
	void connectToClient();
	void closeSocket();
	void run();
	void handleLogin(const std::string& email, const std::string& password);
private:
	DummyChef(Administrator* administrator):serverSocket(INVALID_SOCKET),clientSocket(INVALID_SOCKET) 
	{ this->administrator = administrator; };
	~DummyChef() { Administrator::destroyInstance(); closeSocket();};
	std::vector<Utilizator*>utilizatori;
	std::vector<Ingrediente*>ingrediente;
	std::vector<Reteta*>retete;
	std::vector<Furnizor*>furnizori;
	Administrator* administrator;
	static DummyChef* app;
	SOCKET serverSocket;
	SOCKET clientSocket;

};

