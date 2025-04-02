CREATE DATABASE DummyChefDB


CREATE TABLE Utilizatori (
    ID INT IDENTITY(1,1) PRIMARY KEY,  -- Cheie primarã auto-incrementatã
    Nume NVARCHAR(50) NOT NULL,
    Prenume NVARCHAR(50) NOT NULL,
    NumeUtilizator NVARCHAR(50) UNIQUE NOT NULL,
    Parola NVARCHAR(255) NOT NULL,
    NrTelefon NVARCHAR(20) UNIQUE NOT NULL,
    DataNasterii DATE NOT NULL,
    Email NVARCHAR(100) UNIQUE NOT NULL,
    AdresaLivrare NVARCHAR(200) ,
    TipUtilizator NVARCHAR(20)  CHECK (TipUtilizator IN ('Client', 'Bucatar'))
);

CREATE TABLE Produse (
    ID INT IDENTITY(1,1) PRIMARY KEY,
    Nume NVARCHAR(255) NOT NULL,
    Pret DECIMAL(10,2) NOT NULL CHECK (Pret >= 0),
    Stoc INT NOT NULL CHECK (Stoc >= 0),
    DataAdaugarii DATETIME DEFAULT GETDATE()
);

