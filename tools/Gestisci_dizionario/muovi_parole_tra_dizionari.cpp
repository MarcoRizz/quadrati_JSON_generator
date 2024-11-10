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

std::string rimuoviAccenti(const std::string& input);
void ensureFileExists(const std::string& filePath);

// Inizializzazione di un array di std::string
std::vector<std::string> parole_da_accettare = {

        };

// Inizializzazione di un array di std::string
std::vector<std::string> parole_da_bonus = {
        "paola"
        };

std::ifstream dizionario_txt_accettate(DIZIONARIO_TXT_ACCETTATE_PATH);
std::ifstream dizionario_txt_bonus(DIZIONARIO_TXT_BONUS_PATH);
std::ifstream dizionario_txt_rifiutate(DIZIONARIO_TXT_RIFIUTATE_PATH);

Dizionario dizionario;

int main() {

    ensureFileExists(DIZIONARIO_TXT_ACCETTATE_PATH);
    ensureFileExists(DIZIONARIO_TXT_BONUS_PATH);
    ensureFileExists(DIZIONARIO_TXT_RIFIUTATE_PATH);

     //apro il dizionario
    if (!dizionario.caricaDaFileCompatto(PERCORSO_TREE_DICTIONARY)) {
        std::cout << "errore nell'apertura del file al percorso: " << PERCORSO_TREE_DICTIONARY << std::endl;
        return -1;
    }
    
    // Sposto le parole di parole_da_accettare in dizionario_parole_accettate
    for (const auto& parola : parole_da_accettare) {
        auto parola_utc8 = rimuoviAccenti(parola);
        
        aggiungiParola(DIZIONARIO_TXT_ACCETTATE_PATH, parola_utc8);
        rimuoviParola(DIZIONARIO_TXT_BONUS_PATH, parola_utc8);
        rimuoviParola(DIZIONARIO_TXT_RIFIUTATE_PATH, parola_utc8);
        dizionario.inserisciParola(parola_utc8);
        std::cout << parola << std::endl;
    }
    std::cout << "Parole aggiunte correttamente" << std::endl;

    // Sposto le parole di dizionario_txt_bonus in dizionario_parole_bonus
    for (const auto& parola : parole_da_bonus) {
        auto parola_utc8 = rimuoviAccenti(parola);
        
        if (rimuoviParola(DIZIONARIO_TXT_ACCETTATE_PATH, parola_utc8)) {
            std::cout << "rimossa correttamente da accettate" << std::endl;
        }
        else {
            std::cout << "non c'era in accettate" << std::endl;
        }

        aggiungiParola(DIZIONARIO_TXT_BONUS_PATH, parola_utc8);

        if (rimuoviParola(DIZIONARIO_TXT_RIFIUTATE_PATH, parola_utc8)) {
            std::cout << "rimossa correttamente da rifiutate" << std::endl;
        }
        else {
            std::cout << "non c'era in rifiutate" << std::endl;
        }
        dizionario.inserisciParola(parola_utc8);
        std::cout << parola << std::endl;
    }
    std::cout << "Parole aggiunte correttamente come bonus" << std::endl;

    dizionario.salvaInFileCompatto(PERCORSO_TREE_DICTIONARY);
}

// Mappa dei caratteri accentati e le corrispondenti vocali senza accento (con wchar_t)
std::string rimuoviAccenti(const std::string& input) {
    // Converti std::string in std::wstring
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wide_input = converter.from_bytes(input);

    // Definisci la mappa dei caratteri accentati
    static const std::unordered_map<wchar_t, wchar_t> accenti{
        //{'à', 'a'}, {'è', 'e'}, {'é', 'e'}, {'ì', 'i'}, {'ò', 'o'}, {'ù', 'u'},
        //{'À', 'A'}, {'È', 'E'}, {'É', 'E'}, {'Ì', 'I'}, {'Ò', 'O'}, {'Ù', 'U'}
        {L'\u00E0', L'a'}, {L'\u00E8', L'e'}, {L'\u00E9', L'e'}, {L'\u00EC', L'i'}, {L'\u00F2', L'o'}, {L'\u00F9', L'u'},
        {L'\u00C0', L'A'}, {L'\u00C8', L'E'}, {L'\u00C9', L'E'}, {L'\u00CC', L'I'}, {L'\u00D2', L'O'}, {L'\u00D9', L'U'}
    };

    std::wstring wide_output;
    wide_output.reserve(wide_input.size()); // Prealloca la memoria

    for (wchar_t c : wide_input) {
        if (accenti.count(c)) {
            wide_output += accenti.at(c); // Sostituisci se è accentato
        } else if (c >= 0 && c < 128) {
            wide_output += c; // Aggiungi i caratteri ASCII non accentati
        }
    }

    // Converti std::wstring in std::string
    std::string output = converter.to_bytes(wide_output);
    return output;
}

void ensureFileExists(const std::string& filePath) {
    if (!std::filesystem::exists(filePath)) {
        std::ofstream outFile(filePath);
        if (!outFile) {
            std::cerr << "Errore: impossibile creare il file " << filePath << "\n";
        } else {
            std::cout << "File creato: " << filePath << "\n";
        }
        outFile.close();
    }
}