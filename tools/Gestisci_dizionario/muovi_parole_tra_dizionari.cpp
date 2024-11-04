#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "../../Classes/gestisci_dizionari.h"
#include "../../Classes/Dizionario.h"

#define PERCORSO_TREE_DICTIONARY "..\\Dizionari\\dizionario.json"
#define DIZIONARIO_TXT_ACCETTATE_PATH "tools\\Dizionari\\dizionario_parole_accettate.txt"
#define DIZIONARIO_TXT_BONUS_PATH "tools\\Dizionari\\dizionario_parole_bonus.txt"
#define DIZIONARIO_TXT_RIFIUTATE_PATH "tools\\Dizionari\\dizionario_parole_rifiutate.txt"

using String = std::string;

String rimuoviAccenti(const String& input);

// Inizializzazione di un array di std::string
std::vector<std::string> parole_da_accettare = {
        "lussa",
        "suasi",
        "sfusa",
        "lussai"
        };

// Inizializzazione di un array di std::string
std::vector<std::string> parole_da_bonus = {
        "sussi"
        };

std::ifstream dizionario_txt_accettate(DIZIONARIO_TXT_ACCETTATE_PATH);
std::ifstream dizionario_txt_bonus(DIZIONARIO_TXT_BONUS_PATH);
std::ifstream dizionario_txt_rifiutate(DIZIONARIO_TXT_RIFIUTATE_PATH);

Dizionario dizionario;

int main() {

     //apro il dizionario
    if (!dizionario.caricaDaFileCompatto(PERCORSO_TREE_DICTIONARY)) {
        std::cout << "errore nell'apertura del file al percorso: " << PERCORSO_TREE_DICTIONARY << std::endl;
        return -1;
    }
    
    // Sposto le parole di parole_da_accettare in dizionario_parole_accettate
    for (const auto& parola : parole_da_accettare) {
        auto parola_utc8 = rimuoviAccenti(parola);
        
        aggiungiParola(dizionario_txt_accettate, DIZIONARIO_TXT_ACCETTATE_PATH, parola_utc8);
        rimuoviParola(dizionario_txt_bonus, DIZIONARIO_TXT_BONUS_PATH, parola_utc8);
        rimuoviParola(dizionario_txt_rifiutate, DIZIONARIO_TXT_RIFIUTATE_PATH, parola_utc8);
        dizionario.inserisciParola(parola_utc8);
        std::cout << parola << std::endl;
    }
    std::cout << "Parole aggiunte correttamente" << std::endl;

    // Sposto le parole di dizionario_txt_bonus in dizionario_parole_bonus
    for (const auto& parola : parole_da_bonus) {
        auto parola_utc8 = rimuoviAccenti(parola);
        
        rimuoviParola(dizionario_txt_accettate, DIZIONARIO_TXT_ACCETTATE_PATH, parola_utc8);
        aggiungiParola(dizionario_txt_bonus, DIZIONARIO_TXT_BONUS_PATH, parola_utc8);
        rimuoviParola(dizionario_txt_rifiutate, DIZIONARIO_TXT_RIFIUTATE_PATH, parola_utc8);
        dizionario.inserisciParola(parola_utc8);
        std::cout << parola << std::endl;
    }
    std::cout << "Parole aggiunte correttamente come bonus" << std::endl;

    dizionario.salvaInFileCompatto(PERCORSO_TREE_DICTIONARY);
}

// Mappa dei caratteri accentati e le corrispondenti vocali senza accento
String rimuoviAccenti(const String& input) {
    static const std::unordered_map<char, char> accenti{
        {'à', 'a'}, {'è', 'e'}, {'é', 'e'}, {'ì', 'i'}, {'ò', 'o'}, {'ù', 'u'},
        {'À', 'A'}, {'È', 'E'}, {'É', 'E'}, {'Ì', 'I'}, {'Ò', 'O'}, {'Ù', 'U'}
    };

    String output;
    output.reserve(input.size()); // Prealloca la memoria

    for (char c : input) {
        if (accenti.count(c)) {
            output += accenti.at(c); // Sostituisci se è accentato
        } else if (c >= 0 && c < 128) {
            output += c; // Aggiungi i caratteri ASCII non accentati
        }
    }
    return output;
}