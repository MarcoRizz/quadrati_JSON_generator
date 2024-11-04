#include <string>
#include <cstdlib>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

#include "gestisci_dizionari.h"
//#include "generate_JSON.h"

ParolaStatus ask_the_boss(const std::string& parola) {
    char input;

    while (true) {
        std::cout << "Inserisci il comando per la parola \"" << parola << "\" (y: accettata, b: bonus, n: rifiutata, s: cerca su Google): ";
        std::cin >> input;

        switch (input) {
            case 'y':
                return ParolaStatus::Accettata;

            case 'b':
                return ParolaStatus::Bonus;

            case 'n':
                return ParolaStatus::Rifiutata;

            case 's': {
                // Definisce l'URL e apre la pagina web
                std::string url = "https://www.google.com/search?q=" + parola + "+vocabolario";
                std::system(("start " + url).c_str());
                break;
            }

            default:
                std::cout << "Comando non valido. Riprova.\n";
                break;
        }
    }
}

// Funzione per verificare se una parola è presente nel dizionario
bool contieneParola(std::ifstream& file, const std::string& parola) {
    std::string line;
    
    // Riposiziona il puntatore all'inizio del file
    file.clear(); // Resetta eventuali flag (EOF, ecc.)
    file.seekg(0); // Torna all'inizio del file

    while (std::getline(file, line)) {
        if (line == parola) {
            return true; // La parola è presente
        }
    }
    return false; // La parola non è stata trovata
}

// Funzione per rimuovere una parola dal dizionario
bool rimuoviParola(std::ifstream& file, const std::string& filePath, const std::string& parola) {
    std::vector<std::string> parole;
    std::string line;
    bool found = false;

    // Riposiziona il puntatore all'inizio del file
    file.clear(); // Resetta eventuali flag
    file.seekg(0); // Torna all'inizio del file

    // Legge tutte le parole nel file e salta quella da rimuovere
    while (std::getline(file, line)) {
        if (line == parola) {
            found = true;
            continue; // Salta l'inserimento della parola da rimuovere
        }
        parole.push_back(line);
    }

    if (!found) {
        std::cout << "La parola \"" << parola << "\" non è presente nel dizionario.\n";
        return false;
    }

    // Sovrascrive il file con il dizionario aggiornato
    std::ofstream outFile(filePath);
    for (const auto& p : parole) {
        outFile << p << "\n"; // Scrive ogni parola nel file
    }
    return true;
}

// Funzione per aggiungere una parola nel dizionario mantenendo l'ordine alfabetico
bool aggiungiParola(std::ifstream& file, const std::string& filePath, const std::string& parola) {
    std::vector<std::string> parole;
    std::string line;
    bool alreadyExists = false;

    // Riposiziona il puntatore all'inizio del file
    file.clear(); // Resetta eventuali flag
    file.seekg(0); // Torna all'inizio del file

    // Legge tutte le parole dal file e inserisce la nuova parola nell'ordine corretto
    while (std::getline(file, line)) {
        if (line == parola) {
            alreadyExists = true;
            break; // Esce se la parola esiste già
        }
        if (line > parola && !alreadyExists) {
            parole.push_back(parola); // Aggiunge la parola prima di quella maggiore
            alreadyExists = true;
        }
        parole.push_back(line); // Aggiunge la parola letta dal file
    }

    // Se la parola non era presente e non è stata inserita, aggiungila alla fine
    if (!alreadyExists) {
        parole.push_back(parola);
    }

    // Sovrascrive il file con il dizionario aggiornato
    std::ofstream outFile(filePath);
    for (const auto& p : parole) {
        outFile << p << "\n"; // Scrive ogni parola nel file
    }
    return true;
}