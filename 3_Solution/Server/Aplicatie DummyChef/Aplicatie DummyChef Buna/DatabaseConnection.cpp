#include "DatabaseConnection.h"
#include <iostream>
#include "Utilizator.h"
#include "Client.h"
#include "Administrator.h"
#include "Bucatar.h"

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
    wchar_t buffer[256];
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
