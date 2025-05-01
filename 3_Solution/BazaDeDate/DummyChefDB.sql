CREATE DATABASE DummyChefDB


CREATE TABLE Utilizatori (
    ID INT IDENTITY(1,1) PRIMARY KEY,  -- Cheie primară auto-incrementată
    Nume NVARCHAR(50) NOT NULL,
    Prenume NVARCHAR(50) NOT NULL,
    NumeUtilizator NVARCHAR(50) UNIQUE NOT NULL,
    Parola NVARCHAR(255) NOT NULL,
    NrTelefon NVARCHAR(20) UNIQUE NOT NULL,
    DataNasterii DATE NOT NULL,
    Email NVARCHAR(100) UNIQUE NOT NULL,
    AdresaLivrare NVARCHAR(200) ,
	Experienta INT,
	LinkDemonstrativ NVARCHAR(50),
    TipUtilizator NVARCHAR(20)  CHECK (TipUtilizator IN ('Client', 'Bucatar'))
);


CREATE TABLE Ingrediente (
    ID INT IDENTITY(1,1) PRIMARY KEY,
    Nume NVARCHAR(255) NOT NULL,
    Pret DECIMAL(10,2) NOT NULL CHECK (Pret >= 0),
    DataAdaugarii DATETIME DEFAULT GETDATE()
);

CREATE TABLE Stoc(
	ID INT IDENTITY(1,1) PRIMARY KEY,
	IngredientID INT NOT NULL FOREIGN KEY REFERENCES Ingrediente(ID),
	Cantiate INT
);

CREATE TABLE Furnizori (
    ID INT IDENTITY(1,1) PRIMARY KEY,
    Nume NVARCHAR(255) NOT NULL,
    Telefon NVARCHAR(20) UNIQUE NOT NULL,
    Email NVARCHAR(100) UNIQUE NOT NULL,
    AdresaLivrare NVARCHAR(200) 
);

CREATE TABLE Retete(
	ID INT IDENTITY(1,1) PRIMARY KEY,
	Denumire NVARCHAR(100) NOT NULL,
	TimpPreparare NVARCHAR(5) NOT NULL,
	PasiPreparare NVARCHAR(1000) NOT NULL,
	IDBucatar INT NOT NULL FOREIGN KEY REFERENCES Utilizatori(ID)
);

CREATE TABLE ReteteIngrediente (
    ID INT IDENTITY(1,1) PRIMARY KEY,
    RetetaID INT NOT NULL FOREIGN KEY REFERENCES Retete(ID),
    IngredientID INT NOT NULL FOREIGN KEY REFERENCES Ingrediente(ID),
    Cantitate NVARCHAR(50) NOT NULL
);


CREATE TABLE PreferinteClienti (
    ID INT IDENTITY(1,1) PRIMARY KEY,
    IDClient INT NOT NULL FOREIGN KEY REFERENCES Utilizatori(ID),
    PreferinteAlimentare NVARCHAR(500),
    Alergii NVARCHAR(100),
    OraLivrare NVARCHAR(20) NOT NULL CHECK (OraLivrare IN (
        'Orice_ora', 
        'Dimineata(8-12)', 
        'Pranz(12-15)', 
        'Seara(15-20)', 
        'Noapte(20-24)'
    )),
    PreferintaPret NVARCHAR(20) NOT NULL CHECK (PreferintaPret IN (
        'Orice_pret', 
        'Economic', 
        'Moderat', 
        'Premium'
    )),
    Notite NVARCHAR(500),
    DataInregistrare DATETIME DEFAULT GETDATE()
);


INSERT INTO Furnizori (Nume, Telefon, Email, AdresaLivrare)
VALUES (N'Furnizor Standard', N'0712345678', N'furnizor@example.com', N'Soseaua Furnizorilor 1, Bucuresti');


INSERT INTO Stoc (IngredientID, Cantiate) VALUES
(1, 100);  -- Ou

INSERT INTO Ingrediente (Nume, Pret) VALUES 
(N'Ou', 1.50);
