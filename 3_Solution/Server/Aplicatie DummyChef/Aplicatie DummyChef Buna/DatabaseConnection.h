#pragma once
#include <windows.h>
#include <sqltypes.h>
#include <sql.h>
#include <sqlext.h>
#include <vector>
#include <string>
#include <stdexcept>
#include <codecvt>

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