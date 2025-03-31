#include "DatabaseConnection.h"
#include <iostream>

DatabaseConnection::DatabaseConnection(const std::wstring& server,
    const std::wstring& database,
    const std::wstring& username,
    const std::wstring& password)
    : server(server), database(database), username(username), password(password),
    env(nullptr), dbc(nullptr), isConnected(false) {
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &env);
    ThrowIfFailed(ret, L"Error allocating ODBC environment handle");
    ret = SQLSetEnvAttr(env, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);
    ThrowIfFailed(ret, L"Error setting ODBC version");
}

DatabaseConnection::~DatabaseConnection() {
    Disconnect();
    if (env) {
        SQLFreeHandle(SQL_HANDLE_ENV, env);
        env = nullptr;
    }
}

void DatabaseConnection::Connect() {
    if (isConnected) return;

    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_DBC, env, &dbc);
    ThrowIfFailed(ret, L"Error allocating connection handle");

    std::wstring connStr = L"DRIVER={SQL Server};SERVER=" + server +
        L";DATABASE=" + database +
        L";UID=" + username +
        L";PWD=" + password + L";";

    ret = SQLDriverConnectW(dbc, nullptr, (SQLWCHAR*)connStr.c_str(), SQL_NTS,
        nullptr, 0, nullptr, SQL_DRIVER_COMPLETE);
    ThrowIfFailed(ret, L"Error connecting to SQL Server");

    isConnected = true;
    std::wcout << L"Successfully connected to database!" << std::endl;
}

void DatabaseConnection::Disconnect() {
    if (!isConnected) return;

    SQLDisconnect(dbc);
    SQLFreeHandle(SQL_HANDLE_DBC, dbc);
    dbc = nullptr;
    isConnected = false;
    std::wcout << L"Disconnected from database." << std::endl;
}

void DatabaseConnection::ExecuteNonQuery(const std::wstring& sqlQuery) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt = nullptr;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    SQLRETURN ret = SQLExecDirectW(stmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
    ThrowIfFailed(ret, L"Error executing SQL query", SQL_HANDLE_STMT, stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

std::vector<std::vector<std::wstring>> DatabaseConnection::ExecuteQuery(const std::wstring& sqlQuery) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt = nullptr;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    SQLRETURN ret = SQLExecDirectW(stmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
    ThrowIfFailed(ret, L"Error executing SQL query", SQL_HANDLE_STMT, stmt);

    std::vector<std::vector<std::wstring>> results;
    SQLSMALLINT numColumns = 0;
    SQLNumResultCols(stmt, &numColumns);

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        std::vector<std::wstring> row;
        for (SQLSMALLINT i = 1; i <= numColumns; i++) {
            SQLWCHAR columnData[256];
            SQLLEN indicator;
            SQLGetData(stmt, i, SQL_C_WCHAR, columnData, sizeof(columnData), &indicator);
            row.push_back(indicator == SQL_NULL_DATA ? L"NULL" : columnData);
        }
        results.push_back(row);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return results;
}

void DatabaseConnection::InsertProduct(const std::wstring& nume, float pret, int stoc) {
    std::wstring sqlQuery =
        L"INSERT INTO Produse (Nume, Pret, Stoc, DataAdaugarii) "
        L"VALUES (N'" + nume + L"', " + std::to_wstring(pret) +
        L", " + std::to_wstring(stoc) + L", GETDATE())";
    ExecuteNonQuery(sqlQuery);
}

void DatabaseConnection::ThrowIfFailed(SQLRETURN ret,
    const std::wstring& errorMessage,
    SQLSMALLINT handleType,
    SQLHANDLE handle) {
    if (!SQL_SUCCEEDED(ret)) {
        SQLHANDLE effectiveHandle = handle ? handle : dbc;
        SQLSMALLINT effectiveType = handle ? handleType : SQL_HANDLE_DBC;

        SQLWCHAR sqlState[6] = { 0 };
        SQLINTEGER nativeError = 0;
        SQLWCHAR messageText[SQL_MAX_MESSAGE_LENGTH] = { 0 };
        SQLSMALLINT textLength = 0;

        SQLGetDiagRecW(effectiveType, effectiveHandle, 1, sqlState, &nativeError,
            messageText, SQL_MAX_MESSAGE_LENGTH, &textLength);

        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string fullError = converter.to_bytes(
            errorMessage + L"\nSQL State: " + sqlState +
            L"\nODBC Error: " + messageText
        );

        throw std::runtime_error(fullError);
    }
}