#include <codecvt>
#include <locale>
#include <string>
#include "dictionary_tree_generator.h"

//(TEMP)
#define DICTIONARY_PATH1 "..\\Dizionari\\dizionario_parole.txt"
#define DICTIONARY_PATH2 "..\\Dizionari\\dizionario_coniugazioni.txt"

std::vector<String> dictionaryPaths = {DICTIONARY_PATH1, DICTIONARY_PATH2}; // Aggiungi quanti file desideri
String line;

String rimuoviAccenti(const String& input);

int main() {
    auto timer_start = std::chrono::high_resolution_clock::now();
    auto timer_end = timer_start;
    std::chrono::duration<double, std::milli> duration = timer_end - timer_start;
    Dizionario dizionario;

    //---------------------------------------------
    //sezione di creazione del dizionario

    // Itera su ogni percorso di file nel vettore
    for (const auto& path : dictionaryPaths) {
        std::ifstream file(path);

        // Verifica se il file è stato aperto correttamente
        if (!file.is_open()) {
            std::cout << "Errore nell'apertura del dizionario: " << path << std::endl;
            continue; // Passa al file successivo
        }

        // Legge e inserisce le parole dal file corrente
        while (std::getline(file, line)) {
            std::string parolaPulita = rimuoviAccenti(line);
            dizionario.inserisciParola(parolaPulita);
        }

        // Chiude il file al termine della lettura
        file.close();

        timer_end = std::chrono::high_resolution_clock::now();
        duration = timer_end - timer_start;
        std::cout << path << " added. Operation time: " << duration.count() << " ms" << std::endl;
        timer_start = timer_end;
    }

    // Ricerca di parole
    String paroleDaCercare[] = {"cane", "casa", "caro", "care", "cat", "cate", "can"};
    for (const auto& parola : paroleDaCercare) {
        if (dizionario.cercaParola(parola)) {
            std::cout << "La parola \"" << parola << "\" esiste nel dizionario." << std::endl;
        } else {
            std::cout << "La parola \"" << parola << "\" non esiste nel dizionario." << std::endl;
        }
    }

    // Conteggio delle parole nel dizionario
    int totaleParole = dizionario.contaParole();
    std::cout << "\nNumero totale di parole nel dizionario: " << totaleParole << std::endl;

    timer_start = std::chrono::high_resolution_clock::now();

    // Serializzazione del dizionario
    if (dizionario.salvaInFileCompatto(PERCORSO_TREE_DICTIONARY)) {
        std::cout << "Dizionario salvato correttamente in \"" << PERCORSO_TREE_DICTIONARY << "\"." << std::endl;
    }

    timer_end = std::chrono::high_resolution_clock::now();
    duration = timer_end - timer_start;
    std::cout << "Operation time: " << duration.count() << " ms" << std::endl;
    timer_start = std::chrono::high_resolution_clock::now();

    // Creazione di un nuovo dizionario e deserializzazione da file
    Dizionario nuovoDizionario;
    if (nuovoDizionario.caricaDaFileCompatto(PERCORSO_TREE_DICTIONARY)) {
        std::cout << "Dizionario caricato correttamente da \"" << PERCORSO_TREE_DICTIONARY << "\"." << std::endl;
    }

    timer_end = std::chrono::high_resolution_clock::now();
    duration = timer_end - timer_start;
    std::cout << "Operation time: " << duration.count() << " ms" << std::endl;

    // Verifica delle parole nel nuovo dizionario
    for (const auto& parola : paroleDaCercare) {
        if (nuovoDizionario.cercaParola(parola)) {
            std::cout << "La parola \"" << parola << "\" esiste nel nuovo dizionario." << std::endl;
        } else {
            std::cout << "La parola \"" << parola << "\" non esiste nel nuovo dizionario." << std::endl;
        }
    }

    // Conteggio delle parole nel nuovo dizionario
    int totaleParoleNuovo = nuovoDizionario.contaParole();
    std::cout << "\nNumero totale di parole nel nuovo dizionario: " << totaleParoleNuovo << std::endl;

    return 0;
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