#pragma once
#include <string>
#include <iostream>

class Comanda {
private:
    int idComanda;
    int idClient;
    std::string numeReteta;
    std::string dataExecutare;

public:
    Comanda(int id, int idClient, const std::string& reteta, const std::string& data)
        : idComanda(id), idClient(idClient), numeReteta(reteta), dataExecutare(data) {
    }
    int GetIdComanda() const { return idComanda; }
    int GetIdClient() const { return idClient; }
    std::string GetNumeReteta() const { return numeReteta; }
    std::string GetDataExecutare() const { return dataExecutare; }
    void PrintComanda() const {
        std::cout << "-----------------------------\n";
        std::cout << "Comanda #" << idComanda << "\n";
        std::cout << "ID Client: " << idClient << "\n";
        std::cout << "Reteta: " << numeReteta << "\n";
        std::cout << "Data executarii: " << dataExecutare << "\n";
    }
};

