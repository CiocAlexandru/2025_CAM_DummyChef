#include "DatabaseConnection.h"
#include <iostream>
#include "Utilizator.h"
#include "Client.h"
#include "Administrator.h"
#include "Bucatar.h"
#include "Furnizor.h"
#include "Reteta.h"
#include "Comanda.h"
#include "Preferinte.h"
#include "Client.h"

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
            SQLWCHAR columnData[2048];
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

void DatabaseConnection::InsertClient(const std::wstring& nume, const std::wstring& prenume,
    const std::wstring& nume_utilizator, const std::wstring& parola,
    const std::wstring& nr_telefon, const std::wstring& data_nasterii,
    const std::wstring& email, const std::wstring& adresa_livrare)
{
    if (!isConnected) {
        throw std::runtime_error("Not connected to database");
    }

    SQLHSTMT stmt = nullptr;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    try {
        std::wstring sqlQuery = L"INSERT INTO Utilizatori (Nume, Prenume, NumeUtilizator, Parola, "
            L"NrTelefon, DataNasterii, Email, AdresaLivrare, Experienta, TipUtilizator) "
            L"VALUES (?, ?, ?, ?, ?, ?, ?, ?, 0, 'Client')"; // Experience set to 0 for clients

        SQLPrepareW(stmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);

        // Bind parameters
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            nume.length(), 0, (SQLPOINTER)nume.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            prenume.length(), 0, (SQLPOINTER)prenume.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            nume_utilizator.length(), 0, (SQLPOINTER)nume_utilizator.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            parola.length(), 0, (SQLPOINTER)parola.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            nr_telefon.length(), 0, (SQLPOINTER)nr_telefon.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            data_nasterii.length(), 0, (SQLPOINTER)data_nasterii.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            adresa_livrare.length(), 0, (SQLPOINTER)adresa_livrare.c_str(), 0, nullptr);

        SQLRETURN ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            ThrowIfFailed(ret, L"Error inserting client", SQL_HANDLE_STMT, stmt);
        }

        std::wcout << L"Client added successfully!" << std::endl;
    }
    catch (...) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}

void DatabaseConnection::InsertChef(const std::wstring& nume, const std::wstring& prenume,
    const std::wstring& nume_utilizator, const std::wstring& parola,
    const std::wstring& nr_telefon, const std::wstring& data_nasterii,
    const std::wstring& email, int experienta ,const std::wstring& link_demonstrativ)
{
    if (!isConnected) {
        throw std::runtime_error("Not connected to database");
    }

    SQLHSTMT stmt = nullptr;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    try {
        std::wstring sqlQuery = L"INSERT INTO Utilizatori (Nume, Prenume, NumeUtilizator, Parola, "
            L"NrTelefon, DataNasterii, Email, Experienta, LinkDemonstrativ, TipUtilizator) "
            L"VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, 'Bucatar')";

        SQLPrepareW(stmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);

        // Bind parameters
        SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            nume.length(), 0, (SQLPOINTER)nume.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            prenume.length(), 0, (SQLPOINTER)prenume.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            nume_utilizator.length(), 0, (SQLPOINTER)nume_utilizator.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            parola.length(), 0, (SQLPOINTER)parola.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            nr_telefon.length(), 0, (SQLPOINTER)nr_telefon.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            data_nasterii.length(), 0, (SQLPOINTER)data_nasterii.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 7, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);
        SQLBindParameter(stmt, 8, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
            0, 0, (SQLPOINTER)&experienta, 0, nullptr);
        SQLBindParameter(stmt, 9, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            link_demonstrativ.length(), 0, (SQLPOINTER)link_demonstrativ.c_str(), 0, nullptr);
        

        SQLRETURN ret = SQLExecute(stmt);
        if (!SQL_SUCCEEDED(ret)) {
            ThrowIfFailed(ret, L"Error inserting chef", SQL_HANDLE_STMT, stmt);
        }

        std::wcout << L"Chef added successfully with experience: " << experienta << L" years!" << std::endl;
    }
    catch (...) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
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


bool DatabaseConnection::VerifyCredentials(const std::wstring& email, const std::wstring& password) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt = nullptr;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring sqlQuery =
        L"SELECT COUNT(*) FROM Utilizatori WHERE Email = ? AND Parola = ?";

    // Prepare the statement
    SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
    ThrowIfFailed(ret, L"Error preparing SQL query", SQL_HANDLE_STMT, stmt);

    // Bind parameters
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 0, 0,
        (SQLPOINTER)email.c_str(), email.length() * sizeof(wchar_t), nullptr);
    ThrowIfFailed(ret, L"Error binding email parameter", SQL_HANDLE_STMT, stmt);

    ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 0, 0,
        (SQLPOINTER)password.c_str(), password.length() * sizeof(wchar_t), nullptr);
    ThrowIfFailed(ret, L"Error binding password parameter", SQL_HANDLE_STMT, stmt);

    // Execute
    ret = SQLExecute(stmt);
    ThrowIfFailed(ret, L"Error executing SQL query", SQL_HANDLE_STMT, stmt);

    // Get result
    int count = 0;
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &count, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return count > 0;
}

std::wstring DatabaseConnection::GetUserType(const std::wstring& email) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt = nullptr;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring sqlQuery =
        L"SELECT TipUtilizator FROM Utilizatori WHERE Email = ?";

    // Prepare the statement
    SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
    ThrowIfFailed(ret, L"Error preparing SQL query", SQL_HANDLE_STMT, stmt);

    // Bind parameter
    ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR, 0, 0,
        (SQLPOINTER)email.c_str(), email.length() * sizeof(wchar_t), nullptr);
    ThrowIfFailed(ret, L"Error binding email parameter", SQL_HANDLE_STMT, stmt);

    // Execute
    ret = SQLExecute(stmt);
    ThrowIfFailed(ret, L"Error executing SQL query", SQL_HANDLE_STMT, stmt);

    // Get result
    std::wstring userType = L"";
    wchar_t buffer[256];
    SQLLEN indicator;

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_WCHAR, buffer, sizeof(buffer), &indicator);
        if (indicator != SQL_NULL_DATA) {
            userType = buffer;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return userType;
}






// Verifică dacă un utilizator există
bool DatabaseConnection::UserExists(const std::wstring& email) {
    if (!isConnected) throw std::runtime_error("Database not connected");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT COUNT(*) FROM Utilizatori WHERE Email = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);

    SQLExecute(stmt);

    int count = 0;
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &count, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return count > 0;
}

// Obține utilizatorul după credențiale
std::unique_ptr<Utilizator> DatabaseConnection::GetUserByCredentials(const std::wstring& email, const std::wstring& password) {
    if (!isConnected) throw std::runtime_error("Database not connected");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT TipUtilizator, Nume, Prenume, Email FROM Utilizatori WHERE Email = ? AND Parola = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    // Bind parameters
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        password.length(), 0, (SQLPOINTER)password.c_str(), 0, nullptr);

    SQLExecute(stmt);

    wchar_t userType[50], nume[50], prenume[50], userEmail[100];
    SQLLEN indicator;

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_WCHAR, userType, sizeof(userType), &indicator);
        SQLGetData(stmt, 2, SQL_C_WCHAR, nume, sizeof(nume), &indicator);
        SQLGetData(stmt, 3, SQL_C_WCHAR, prenume, sizeof(prenume), &indicator);
        SQLGetData(stmt, 4, SQL_C_WCHAR, userEmail, sizeof(userEmail), &indicator);

        // Convertim la string
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string typeStr = converter.to_bytes(userType);

        if (typeStr == "Client") {
            return std::make_unique<Client>(
                converter.to_bytes(nume),
                converter.to_bytes(prenume),
                "", // nume_utilizator
                "", // parola
                "", // nr_telefon
                "", // data_nasterii
                converter.to_bytes(userEmail),
                ""  // adresa_livrare
            );
        }

        else if (typeStr == "Bucatar") {  // Adaugă acest caz
            return std::make_unique<Bucatar>(
                converter.to_bytes(nume),
                converter.to_bytes(prenume),
                "", // nume_utilizator
                "", // parola
                "", // nr_telefon
                "", // data_nasterii
                converter.to_bytes(userEmail),
                0,//experienta
                "" //linkdemo
            );
        }
        // Poți adăuga și alte tipuri de utilizatori aici
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return nullptr;
}




std::wstring DatabaseConnection::GetPasswordByEmail(const std::wstring& email) {
    if (!isConnected) throw std::runtime_error("Database not connected");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT Parola, TipUtilizator FROM Utilizatori WHERE Email = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    // Bind parameter
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
        email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);

    SQLExecute(stmt);

    wchar_t password[100];
    wchar_t userType[50];
    SQLLEN passwordIndicator, typeIndicator;

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        // Get password and user type
        SQLGetData(stmt, 1, SQL_C_WCHAR, password, sizeof(password), &passwordIndicator);
        SQLGetData(stmt, 2, SQL_C_WCHAR, userType, sizeof(userType), &typeIndicator);

        if (passwordIndicator != SQL_NULL_DATA) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return password; // Return the password if found
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return L""; // Return empty string if not found
}

void DatabaseConnection::InsertPreferinte(int idClient,
    const std::wstring& preferinteAlimentare,
    const std::wstring& alergii,
    const std::wstring& oraLivrare,
    const std::wstring& preferintaPret,
    const std::wstring& notite)
{
    if (!isConnected) {
        throw std::runtime_error("Not connected to database");
    }

    std::wstring sqlQuery = L"INSERT INTO PreferinteClienti (IDClient, PreferinteAlimentare, Alergii, OraLivrare, PreferintaPret, Notite)VALUES (?, ?, ?, ?, ?, ?)";

    SQLHSTMT stmt;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    ThrowIfFailed(ret, L"Failed to allocate statement handle");

    try {
        // Bind parameters
        ret = SQLPrepare(stmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
        ThrowIfFailed(ret, L"Failed to prepare statement", SQL_HANDLE_STMT, stmt);

        // Bind IDClient
        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idClient, 0, NULL);
        ThrowIfFailed(ret, L"Failed to bind IDClient parameter", SQL_HANDLE_STMT, stmt);

        // Bind PreferinteAlimentare
        SQLWCHAR* prefAlim = const_cast<SQLWCHAR*>(preferinteAlimentare.c_str());
        ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            preferinteAlimentare.length(), 0, prefAlim, 0, NULL);
        ThrowIfFailed(ret, L"Failed to bind PreferinteAlimentare parameter", SQL_HANDLE_STMT, stmt);

        // Bind Alergii
        SQLWCHAR* alerg = const_cast<SQLWCHAR*>(alergii.c_str());
        ret = SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            alergii.length(), 0, alerg, 0, NULL);
        ThrowIfFailed(ret, L"Failed to bind Alergii parameter", SQL_HANDLE_STMT, stmt);

        // Bind OraLivrare
        SQLWCHAR* ora = const_cast<SQLWCHAR*>(oraLivrare.c_str());
        ret = SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            oraLivrare.length(), 0, ora, 0, NULL);
        ThrowIfFailed(ret, L"Failed to bind OraLivrare parameter", SQL_HANDLE_STMT, stmt);

        // Bind PreferintaPret
        SQLWCHAR* pret = const_cast<SQLWCHAR*>(preferintaPret.c_str());
        ret = SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            preferintaPret.length(), 0, pret, 0, NULL);
        ThrowIfFailed(ret, L"Failed to bind PreferintaPret parameter", SQL_HANDLE_STMT, stmt);

        // Bind Notite
        SQLWCHAR* note = const_cast<SQLWCHAR*>(notite.c_str());
        ret = SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            notite.length(), 0, note, 0, NULL);
        ThrowIfFailed(ret, L"Failed to bind Notite parameter", SQL_HANDLE_STMT, stmt);

        // Execute the statement
        ret = SQLExecute(stmt);
        ThrowIfFailed(ret, L"Failed to execute statement", SQL_HANDLE_STMT, stmt);
    }
    catch (...) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


bool DatabaseConnection::UserExistsByUsername(const std::wstring& username) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    try {
        std::wstring query = L"SELECT COUNT(*) FROM Utilizatori WHERE NumeUtilizator = ?";
        SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        ThrowIfFailed(ret, L"Error preparing SQL query", SQL_HANDLE_STMT, stmt);

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            username.length(), 0, (SQLPOINTER)username.c_str(), 0, nullptr);
        ThrowIfFailed(ret, L"Error binding username parameter", SQL_HANDLE_STMT, stmt);

        ret = SQLExecute(stmt);
        ThrowIfFailed(ret, L"Error executing SQL query", SQL_HANDLE_STMT, stmt);

        int count = 0;
        if (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_LONG, &count, 0, nullptr);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return count > 0;
    }
    catch (...) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw;
    }
}

int DatabaseConnection::GetUserIdByUsername(const std::wstring& username) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    try {
        std::wstring query = L"SELECT ID FROM Utilizatori WHERE NumeUtilizator = ?";
        SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        ThrowIfFailed(ret, L"Error preparing SQL query", SQL_HANDLE_STMT, stmt);

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WCHAR,
            username.length(), 0, (SQLPOINTER)username.c_str(), 0, nullptr);
        ThrowIfFailed(ret, L"Error binding username parameter", SQL_HANDLE_STMT, stmt);

        ret = SQLExecute(stmt);
        ThrowIfFailed(ret, L"Error executing SQL query", SQL_HANDLE_STMT, stmt);

        int id = -1;
        if (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_LONG, &id, 0, nullptr);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return id;
    }
    catch (...) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw;
    }
}

bool DatabaseConnection::PreferencesExist(int clientId) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    try {
        std::wstring query = L"SELECT COUNT(*) FROM PreferinteClienti WHERE IDClient = ?";
        SQLRETURN ret = SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
        ThrowIfFailed(ret, L"Error preparing SQL query", SQL_HANDLE_STMT, stmt);

        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
            0, 0, (SQLPOINTER)&clientId, 0, nullptr);
        ThrowIfFailed(ret, L"Error binding clientId parameter", SQL_HANDLE_STMT, stmt);

        ret = SQLExecute(stmt);
        ThrowIfFailed(ret, L"Error executing SQL query", SQL_HANDLE_STMT, stmt);

        int count = 0;
        if (SQLFetch(stmt) == SQL_SUCCESS) {
            SQLGetData(stmt, 1, SQL_C_LONG, &count, 0, nullptr);
        }

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        return count > 0;
    }
    catch (...) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw;
    }
}

void DatabaseConnection::UpdateClientPreferences(int clientId, const std::wstring& preferinteAlimentare,
    const std::wstring& alergii, const std::wstring& oraLivrare,
    const std::wstring& preferintaPret, const std::wstring& notite) 
{
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLRETURN ret = SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    ThrowIfFailed(ret, L"Failed to allocate statement handle");

    try {
        std::wstring sqlQuery = L"UPDATE PreferinteClienti SET PreferinteAlimentare = ?, Alergii = ?, "
            L"OraLivrare = ?, PreferintaPret = ?, Notite = ? WHERE IDClient = ?";
        ret = SQLPrepareW(stmt, (SQLWCHAR*)sqlQuery.c_str(), SQL_NTS);
        ThrowIfFailed(ret, L"Failed to prepare statement", SQL_HANDLE_STMT, stmt);

        // Bind PreferinteAlimentare
        ret = SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            preferinteAlimentare.length(), 0, (SQLPOINTER)preferinteAlimentare.c_str(), 0, nullptr);
        ThrowIfFailed(ret, L"Failed to bind PreferinteAlimentare parameter", SQL_HANDLE_STMT, stmt);

        // Bind Alergii
        ret = SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            alergii.length(), 0, (SQLPOINTER)alergii.c_str(), 0, nullptr);
        ThrowIfFailed(ret, L"Failed to bind Alergii parameter", SQL_HANDLE_STMT, stmt);

        // Bind OraLivrare
        ret = SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            oraLivrare.length(), 0, (SQLPOINTER)oraLivrare.c_str(), 0, nullptr);
        ThrowIfFailed(ret, L"Failed to bind OraLivrare parameter", SQL_HANDLE_STMT, stmt);

        // Bind PreferintaPret
        ret = SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            preferintaPret.length(), 0, (SQLPOINTER)preferintaPret.c_str(), 0, nullptr);
        ThrowIfFailed(ret, L"Failed to bind PreferintaPret parameter", SQL_HANDLE_STMT, stmt);

        // Bind Notite
        ret = SQLBindParameter(stmt, 5, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
            notite.length(), 0, (SQLPOINTER)notite.c_str(), 0, nullptr);
        ThrowIfFailed(ret, L"Failed to bind Notite parameter", SQL_HANDLE_STMT, stmt);

        // Bind IDClient
        ret = SQLBindParameter(stmt, 6, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
            0, 0, (SQLPOINTER)&clientId, 0, nullptr);
        ThrowIfFailed(ret, L"Failed to bind IDClient parameter", SQL_HANDLE_STMT, stmt);

        // Execute the statement
        ret = SQLExecute(stmt);
        ThrowIfFailed(ret, L"Failed to execute statement", SQL_HANDLE_STMT, stmt);

        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    }
    catch (...) {
        SQLFreeHandle(SQL_HANDLE_STMT, stmt);
        throw;
    }
}


int DatabaseConnection::InsertRecipeFromClient(const std::wstring& denumire, const std::wstring& timpPreparare, const std::wstring& pasiPreparare, int userId) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"INSERT INTO Retete (Denumire, TimpPreparare, PasiPreparare, IDBucatar) OUTPUT INSERTED.ID VALUES (?, ?, ?, ?)";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        denumire.length(), 0, (SQLPOINTER)denumire.c_str(), 0, nullptr);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        timpPreparare.length(), 0, (SQLPOINTER)timpPreparare.c_str(), 0, nullptr);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        pasiPreparare.length(), 0, (SQLPOINTER)pasiPreparare.c_str(), 0, nullptr);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER,
        0, 0, &userId, 0, nullptr);

    SQLRETURN ret = SQLExecute(stmt);
    ThrowIfFailed(ret, L"Failed to insert recipe", SQL_HANDLE_STMT, stmt);

    int insertedId = -1;
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &insertedId, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return insertedId;
}

int DatabaseConnection::GetUserIdByEmail(const std::wstring& email) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT ID FROM Utilizatori WHERE Email = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);

    int id = -1;
    SQLExecute(stmt);
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &id, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return id;
}


void DatabaseConnection::InsertRecipeIngredient(int retetaId, const std::wstring& numeIngredient, const std::wstring& cantitate) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    // Obținem ID ingredient
    std::wstring queryIngredient = L"SELECT ID FROM Ingrediente WHERE Nume = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)queryIngredient.c_str(), SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        numeIngredient.length(), 0, (SQLPOINTER)numeIngredient.c_str(), 0, nullptr);

    int ingredientId = -1;
    SQLExecute(stmt);
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &ingredientId, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    if (ingredientId == -1) {
        throw std::runtime_error("Ingredientul \"" + std::string(numeIngredient.begin(), numeIngredient.end()) + "\" nu exista in baza de date.");
    }

    // Inserare in ReteteIngrediente
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);
    std::wstring insertQuery = L"INSERT INTO ReteteIngrediente (RetetaID, IngredientID, Cantitate) VALUES (?, ?, ?)";
    SQLPrepareW(stmt, (SQLWCHAR*)insertQuery.c_str(), SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &retetaId, 0, nullptr);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ingredientId, 0, nullptr);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        cantitate.length(), 0, (SQLPOINTER)cantitate.c_str(), 0, nullptr);

    SQLRETURN ret = SQLExecute(stmt);
    ThrowIfFailed(ret, L"Failed to insert ingredient into ReteteIngrediente", SQL_HANDLE_STMT, stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


int DatabaseConnection::GetFurnizorIdByEmail(const std::wstring& email) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT ID FROM Furnizori WHERE Email = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);

    int id = -1;
    SQLExecute(stmt);
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &id, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return id;
}

int DatabaseConnection::InsertFurnizor(const std::wstring& nume, const std::wstring& telefon,
    const std::wstring& email, const std::wstring& adresa) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"INSERT INTO Furnizori (Nume, Telefon, Email, AdresaLivrare) OUTPUT INSERTED.ID VALUES (?, ?, ?, ?)";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, nume.length(), 0, (SQLPOINTER)nume.c_str(), 0, nullptr);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, telefon.length(), 0, (SQLPOINTER)telefon.c_str(), 0, nullptr);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);
    SQLBindParameter(stmt, 4, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, adresa.length(), 0, (SQLPOINTER)adresa.c_str(), 0, nullptr);

    int id = -1;
    SQLExecute(stmt);
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &id, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return id;
}

int DatabaseConnection::InsertIngredient(const std::wstring& nume, double pret, int furnizorId) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    // Adăugăm IDFurnizor în query
    std::wstring query = L"INSERT INTO Ingrediente (Nume, Pret, DataAdaugarii, IDFurnizor) OUTPUT INSERTED.ID VALUES (?, ?, GETDATE(), ?)";

    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    // Legăm parametrii
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR, nume.length(), 0, (SQLPOINTER)nume.c_str(), 0, nullptr);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_DOUBLE, SQL_DOUBLE, 0, 0, &pret, 0, nullptr);
    SQLBindParameter(stmt, 3, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &furnizorId, 0, nullptr);

    // Executăm și preluăm ID-ul inserat
    int id = -1;
    SQLRETURN ret = SQLExecute(stmt);
    ThrowIfFailed(ret, L"Eroare la inserarea ingredientului", SQL_HANDLE_STMT, stmt);

    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &id, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return id;
}


void DatabaseConnection::InsertStock(int ingredientId, int cantitate) {
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"INSERT INTO Stoc (IngredientID, Cantiate) VALUES (?, ?)";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ingredientId, 0, nullptr);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cantitate, 0, nullptr);

    SQLExecute(stmt);
    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


std::vector<std::wstring> DatabaseConnection::GetRecipesByChefId(int chefId) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT Denumire FROM Retete WHERE IDBucatar = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &chefId, 0, nullptr);

    SQLExecute(stmt);

    std::vector<std::wstring> recipes;
    wchar_t buffer[2048];
    SQLLEN indicator;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_WCHAR, buffer, sizeof(buffer), &indicator);
        if (indicator != SQL_NULL_DATA) {
            recipes.push_back(buffer);
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return recipes;
}


bool DatabaseConnection::IngredientExists(const std::wstring& ingredientName) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt = nullptr;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT COUNT(*) FROM Ingrediente WHERE Nume = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        ingredientName.length(), 0, (SQLPOINTER)ingredientName.c_str(), 0, nullptr);

    SQLExecute(stmt);

    int count = 0;
    if (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &count, 0, nullptr);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return count > 0;
}



void DatabaseConnection::InsertOrder(int clientId, int idReteta) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    std::wstring query = L"INSERT INTO Comenzi (IDClient, IDReteta) VALUES (?, ?)";  // SCHIMBAT

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &clientId, 0, nullptr);
    SQLBindParameter(stmt, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idReteta, 0, nullptr);  // SCHIMBAT

    SQLRETURN ret = SQLExecute(stmt);
    ThrowIfFailed(ret, L"Failed to insert order", SQL_HANDLE_STMT, stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
}


std::vector<std::vector<std::wstring>> DatabaseConnection::GetOrdersByClientId(int clientId) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    // Interogare SQL corectă: se face JOIN între Comenzi și Retete
    std::wstring query =
        L"SELECT R.Denumire, C.DataExecutare "
        L"FROM Comenzi C "
        L"JOIN Retete R ON C.IDReteta = R.ID "
        L"WHERE C.IDClient = " + std::to_wstring(clientId);

    return ExecuteQuery(query);
}



std::vector<Furnizor*> DatabaseConnection::GetAllFurnizori(){
    if (!isConnected) throw std::runtime_error("Not connected to database");

    std::vector<Furnizor*> listaFurnizori;

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT Nume, Telefon, Email, AdresaLivrare FROM Furnizori";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLExecute(stmt);

    wchar_t nume[100], telefon[50], email[100], adresa[200];
    SQLLEN indicator;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_WCHAR, nume, sizeof(nume), &indicator);
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        std::string sNume = converter.to_bytes(nume);

        SQLGetData(stmt, 2, SQL_C_WCHAR, telefon, sizeof(telefon), &indicator);
        std::string sTel = converter.to_bytes(telefon);

        SQLGetData(stmt, 3, SQL_C_WCHAR, email, sizeof(email), &indicator);
        std::string sEmail = converter.to_bytes(email);

        SQLGetData(stmt, 4, SQL_C_WCHAR, adresa, sizeof(adresa), &indicator);
        std::string sAdr = converter.to_bytes(adresa);

        Furnizor* f = new Furnizor(sNume, sTel, sEmail, sAdr);
        listaFurnizori.push_back(f);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return listaFurnizori;
}



std::vector<Utilizator*> DatabaseConnection::GetAllUtilizatori() {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    std::vector<Utilizator*> utilizatori;

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT Nume, Prenume, NumeUtilizator, Parola, NrTelefon, DataNasterii, Email, AdresaLivrare, Experienta, LinkDemonstrativ, TipUtilizator FROM Utilizatori";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLExecute(stmt);

    wchar_t nume[100], prenume[100], numeUtil[100], parola[100], nrTel[50], dataN[50], email[100], adresa[200], tip[20], linkDemo[100];
    int experienta;
    SQLLEN indicator;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_WCHAR, nume, sizeof(nume), &indicator);
        SQLGetData(stmt, 2, SQL_C_WCHAR, prenume, sizeof(prenume), &indicator);
        SQLGetData(stmt, 3, SQL_C_WCHAR, numeUtil, sizeof(numeUtil), &indicator);
        SQLGetData(stmt, 4, SQL_C_WCHAR, parola, sizeof(parola), &indicator);
        SQLGetData(stmt, 5, SQL_C_WCHAR, nrTel, sizeof(nrTel), &indicator);
        SQLGetData(stmt, 6, SQL_C_WCHAR, dataN, sizeof(dataN), &indicator);
        SQLGetData(stmt, 7, SQL_C_WCHAR, email, sizeof(email), &indicator);
        SQLGetData(stmt, 8, SQL_C_WCHAR, adresa, sizeof(adresa), &indicator);
        SQLGetData(stmt, 9, SQL_C_LONG, &experienta, 0, &indicator);
        SQLGetData(stmt, 10, SQL_C_WCHAR, linkDemo, sizeof(linkDemo), &indicator);
        SQLGetData(stmt, 11, SQL_C_WCHAR, tip, sizeof(tip), &indicator);

        std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

        std::string sNume = conv.to_bytes(nume);
        std::string sPrenume = conv.to_bytes(prenume);
        std::string sNumeUtil = conv.to_bytes(numeUtil);
        std::string sParola = conv.to_bytes(parola);
        std::string sTel = conv.to_bytes(nrTel);
        std::string sDataN = conv.to_bytes(dataN);
        std::string sEmail = conv.to_bytes(email);
        std::string sAdresa = conv.to_bytes(adresa);
        std::string sTip = conv.to_bytes(tip);
        std::string sLink = conv.to_bytes(linkDemo);

        if (sTip == "Client") {
            utilizatori.push_back(new Client(sNume, sPrenume, sNumeUtil, sParola, sTel, sDataN, sEmail, sAdresa));
        }
        else if (sTip == "Bucatar") {
            utilizatori.push_back(new Bucatar(sNume, sPrenume, sNumeUtil, sParola, sTel, sDataN, sEmail, experienta, sLink));
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return utilizatori;
}




std::vector<Reteta*> DatabaseConnection::GetAllRetete() {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    std::vector<Reteta*> retete;
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query =
        L"SELECT R.ID, R.Denumire, R.TimpPreparare, R.PasiPreparare, "
        L"I.Nume, RI.Cantitate "
        L"FROM Retete R "
        L"JOIN ReteteIngrediente RI ON R.ID = RI.RetetaID "
        L"JOIN Ingrediente I ON RI.IngredientID = I.ID "
        L"ORDER BY R.ID";

    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLExecute(stmt);

    int idReteta = -1, lastId = -1;
    wchar_t denumire[100], timp[20], pasi[1000], numeIng[100], cantitate[50];
    SQLLEN indicator;
    Reteta* current = nullptr;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &idReteta, 0, &indicator);
        SQLGetData(stmt, 2, SQL_C_WCHAR, denumire, sizeof(denumire), &indicator);
        SQLGetData(stmt, 3, SQL_C_WCHAR, timp, sizeof(timp), &indicator);
        SQLGetData(stmt, 4, SQL_C_WCHAR, pasi, sizeof(pasi), &indicator);
        SQLGetData(stmt, 5, SQL_C_WCHAR, numeIng, sizeof(numeIng), &indicator);
        SQLGetData(stmt, 6, SQL_C_WCHAR, cantitate, sizeof(cantitate), &indicator);

        if (idReteta != lastId) {
            if (current) retete.push_back(current);
            current = new Reteta(
                conv.to_bytes(denumire),
                conv.to_bytes(timp),
                conv.to_bytes(pasi)
            );
            lastId = idReteta;
        }

        std::string sNumeIng = conv.to_bytes(numeIng);
        int cant = std::stoi(conv.to_bytes(cantitate));

        Ingrediente* ing = new Ingrediente(sNumeIng, 0.0, 0); // dummy data
        current->addIngredient(cant, ing);
    }

    if (current) retete.push_back(current);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return retete;
}



std::vector<Comanda*> DatabaseConnection::GetAllComenzi() {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    std::vector<Comanda*> comenzi;
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT C.ID, C.IDClient, R.Denumire, C.DataExecutare "
        L"FROM Comenzi C "
        L"JOIN Retete R ON C.IDReteta = R.ID";

    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLExecute(stmt);

    int idComanda, idClient;
    wchar_t denumire[100], dataExec[100];
    SQLLEN indicator;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_LONG, &idComanda, 0, &indicator);
        SQLGetData(stmt, 2, SQL_C_LONG, &idClient, 0, &indicator);
        SQLGetData(stmt, 3, SQL_C_WCHAR, denumire, sizeof(denumire), &indicator);
        SQLGetData(stmt, 4, SQL_C_WCHAR, dataExec, sizeof(dataExec), &indicator);

        std::string sDenumire = conv.to_bytes(denumire);
        std::string sData = conv.to_bytes(dataExec);

        comenzi.push_back(new Comanda(idComanda, idClient, sDenumire, sData));
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return comenzi;
}





std::vector<Client*> DatabaseConnection::GetClientiCuPreferinte() {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    std::vector<Client*> clienti;
    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"SELECT U.Nume, U.Prenume, U.NumeUtilizator, U.Parola, U.NrTelefon, "
        L"U.DataNasterii, U.Email, U.AdresaLivrare, "
        L"P.PreferinteAlimentare, P.Alergii, P.OraLivrare, P.PreferintaPret, P.Notite "
        L"FROM Utilizatori U "
        L"JOIN PreferinteClienti P ON U.ID = P.IDClient "
        L"WHERE U.TipUtilizator = 'Client'";

    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLExecute(stmt);

    // bufferuri
    wchar_t nume[100], prenume[100], numeUtil[100], parola[100], telefon[50], dataN[50], email[100], adresa[200];
    wchar_t pref[500], alergii[100], ora[50], pret[50], notite[500];
    SQLLEN ind;

    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 1, SQL_C_WCHAR, nume, sizeof(nume), &ind);
        SQLGetData(stmt, 2, SQL_C_WCHAR, prenume, sizeof(prenume), &ind);
        SQLGetData(stmt, 3, SQL_C_WCHAR, numeUtil, sizeof(numeUtil), &ind);
        SQLGetData(stmt, 4, SQL_C_WCHAR, parola, sizeof(parola), &ind);
        SQLGetData(stmt, 5, SQL_C_WCHAR, telefon, sizeof(telefon), &ind);
        SQLGetData(stmt, 6, SQL_C_WCHAR, dataN, sizeof(dataN), &ind);
        SQLGetData(stmt, 7, SQL_C_WCHAR, email, sizeof(email), &ind);
        SQLGetData(stmt, 8, SQL_C_WCHAR, adresa, sizeof(adresa), &ind);
        SQLGetData(stmt, 9, SQL_C_WCHAR, pref, sizeof(pref), &ind);
        SQLGetData(stmt, 10, SQL_C_WCHAR, alergii, sizeof(alergii), &ind);
        SQLGetData(stmt, 11, SQL_C_WCHAR, ora, sizeof(ora), &ind);
        SQLGetData(stmt, 12, SQL_C_WCHAR, pret, sizeof(pret), &ind);
        SQLGetData(stmt, 13, SQL_C_WCHAR, notite, sizeof(notite), &ind);

        // conversie
        std::string sNume = conv.to_bytes(nume);
        std::string sPrenume = conv.to_bytes(prenume);
        std::string sUtil = conv.to_bytes(numeUtil);
        std::string sParola = conv.to_bytes(parola);
        std::string sTel = conv.to_bytes(telefon);
        std::string sDataN = conv.to_bytes(dataN);
        std::string sEmail = conv.to_bytes(email);
        std::string sAdr = conv.to_bytes(adresa);

        std::string sPref = conv.to_bytes(pref);
        std::string sAlergii = conv.to_bytes(alergii);
        std::string sOra = conv.to_bytes(ora);
        std::string sPret = conv.to_bytes(pret);
        std::string sNote = conv.to_bytes(notite);

        Client* client = new Client(
            sNume, sPrenume, sUtil, sParola, sTel, sDataN, sEmail, sAdr
        );

        Preferinte* preferinte = new Preferinte(sPref, sAlergii, sOra, sPret, sNote);
        client->setPreferinte(preferinte); // metodă pe care o vom adăuga imediat în clasa Client

        clienti.push_back(client);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return clienti;
}



bool DatabaseConnection::DeleteUserByEmail(const std::wstring& email) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query = L"DELETE FROM Utilizatori WHERE Email = ?";
    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);

    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_WCHAR, SQL_WVARCHAR,
        email.length(), 0, (SQLPOINTER)email.c_str(), 0, nullptr);

    SQLRETURN ret = SQLExecute(stmt);

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);

    return SQL_SUCCEEDED(ret);
}





bool DatabaseConnection::AreIngredienteInStoc(int idReteta) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmt;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmt);

    std::wstring query =
        L"SELECT I.ID, S.Cantiate, RI.Cantitate "
        L"FROM ReteteIngrediente RI "
        L"JOIN Ingrediente I ON RI.IngredientID = I.ID "
        L"JOIN Stoc S ON I.ID = S.IngredientID "
        L"WHERE RI.RetetaID = ?";

    SQLPrepareW(stmt, (SQLWCHAR*)query.c_str(), SQL_NTS);
    SQLBindParameter(stmt, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idReteta, 0, nullptr);
    SQLExecute(stmt);

    int cantitateStoc = 0;
    wchar_t cantitateNecesarW[50];
    SQLLEN ind;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    while (SQLFetch(stmt) == SQL_SUCCESS) {
        SQLGetData(stmt, 2, SQL_C_LONG, &cantitateStoc, 0, &ind);
        SQLGetData(stmt, 3, SQL_C_WCHAR, cantitateNecesarW, sizeof(cantitateNecesarW), &ind);

        int cantitateNecesar = 0;
        try {
            cantitateNecesar = std::stoi(conv.to_bytes(cantitateNecesarW));
        }
        catch (...) {
            std::cerr << "Cantitate invalidă pentru stoc: " << conv.to_bytes(cantitateNecesarW) << std::endl;
            continue;
        }

        if (cantitateNecesar > cantitateStoc) {
            SQLFreeHandle(SQL_HANDLE_STMT, stmt);
            return false;
        }
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmt);
    return true;
}



void DatabaseConnection::UpdateStocDupaComanda(int idReteta) {
    if (!isConnected) throw std::runtime_error("Not connected to database");

    SQLHSTMT stmtSelect;
    SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmtSelect);

    std::wstring querySelect = L"SELECT IngredientID, Cantitate FROM ReteteIngrediente WHERE RetetaID = ?";
    SQLPrepareW(stmtSelect, (SQLWCHAR*)querySelect.c_str(), SQL_NTS);
    SQLBindParameter(stmtSelect, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &idReteta, 0, nullptr);
    SQLExecute(stmtSelect);

    int ingredientId;
    wchar_t cantitateW[50];
    SQLLEN ind;
    std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;

    while (SQLFetch(stmtSelect) == SQL_SUCCESS) {
        SQLGetData(stmtSelect, 1, SQL_C_LONG, &ingredientId, 0, &ind);
        SQLGetData(stmtSelect, 2, SQL_C_WCHAR, cantitateW, sizeof(cantitateW), &ind);

        std::string cantStr = conv.to_bytes(cantitateW);

        int cantitate = 0;
        try {
            cantitate = std::stoi(cantStr);
        }
        catch (...) {
            std::cerr << "Cantitate invalidă în ReteteIngrediente pentru ID: " << ingredientId << std::endl;
            continue;
        }

        SQLHSTMT stmtUpdate;
        SQLAllocHandle(SQL_HANDLE_STMT, dbc, &stmtUpdate);

        // AICI folosim "Cantiate" din baza de date
        std::wstring updateQuery = L"UPDATE Stoc SET Cantiate = Cantiate - ? WHERE IngredientID = ?";
        SQLPrepareW(stmtUpdate, (SQLWCHAR*)updateQuery.c_str(), SQL_NTS);
        SQLBindParameter(stmtUpdate, 1, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &cantitate, 0, nullptr);
        SQLBindParameter(stmtUpdate, 2, SQL_PARAM_INPUT, SQL_C_LONG, SQL_INTEGER, 0, 0, &ingredientId, 0, nullptr);
        SQLExecute(stmtUpdate);

        SQLFreeHandle(SQL_HANDLE_STMT, stmtUpdate);
    }

    SQLFreeHandle(SQL_HANDLE_STMT, stmtSelect);
}
