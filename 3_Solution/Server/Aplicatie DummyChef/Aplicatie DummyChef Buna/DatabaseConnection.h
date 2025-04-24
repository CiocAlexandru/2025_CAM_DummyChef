#pragma once
#include <windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <codecvt>

class Utilizator;

class DatabaseConnection {
public:
    DatabaseConnection(const std::wstring& server,
        const std::wstring& database,
        const std::wstring& username,
        const std::wstring& password);
    ~DatabaseConnection();

    void Connect();
    void Disconnect();
    void ExecuteNonQuery(const std::wstring& sqlQuery);
    std::vector<std::vector<std::wstring>> ExecuteQuery(const std::wstring& sqlQuery);
    void InsertProduct(const std::wstring& nume, float pret, int stoc);
    void InsertClient(const std::wstring& nume, const std::wstring& prenume,
        const std::wstring& nume_utilizator, const std::wstring& parola,
        const std::wstring& nr_telefon, const std::wstring& data_nasterii,
        const std::wstring& email, const std::wstring& adresa_livrare);
    void InsertChef(const std::wstring& nume, const std::wstring& prenume,
        const std::wstring& nume_utilizator, const std::wstring& parola,
        const std::wstring& nr_telefon, const std::wstring& data_nasterii,
        const std::wstring& email, int ani_vechime, const std::wstring& link_demonstrativ);
    // Funcții noi pentru login
    bool VerifyCredentials(const std::wstring& email, const std::wstring& password);
    std::wstring GetUserType(const std::wstring& email); // Returnează tipul utilizatorului (Client/Administrator)

    std::unique_ptr<Utilizator> GetUserByCredentials(const std::wstring& email, const std::wstring& password);
    bool UserExists(const std::wstring& email);

    std::string forgotPassword(const std::string& email);
    std::wstring GetPasswordByEmail(const std::wstring& email);
    void InsertPreferinte(int idClient,const std::wstring& preferinteAlimentare,const std::wstring& alergii,
        const std::wstring& oraLivrare,const std::wstring& preferintaPret,const std::wstring& notite);
    bool UserExistsByUsername(const std::wstring& username);
    int GetUserIdByUsername(const std::wstring& username);
    bool PreferencesExist(int clientId);
    void UpdateClientPreferences(int clientId, const std::wstring& preferinteAlimentare,
        const std::wstring& alergii, const std::wstring& oraLivrare,
        const std::wstring& preferintaPret, const std::wstring& notite);

private:
    std::wstring server, database, username, password;
    SQLHENV env;
    SQLHDBC dbc;
    bool isConnected;

    void ThrowIfFailed(SQLRETURN ret,
        const std::wstring& errorMessage,
        SQLSMALLINT handleType = SQL_HANDLE_DBC,
        SQLHANDLE handle = nullptr);
};