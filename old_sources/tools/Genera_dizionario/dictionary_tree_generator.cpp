#include <codecvt>
#include <locale>
#include <string>
#include "dictionary_tree_generator.h"

//(TEMP)
#define DICTIONARY_PATH1 "..\\Dizionari\\dizionario_parole.txt"
#define DICTIONARY_PATH2 "..\\Dizionari\\dizionario_coniugazioni.txt"

std::vector<std::string> dictionaryPaths = {DICTIONARY_PATH1, DICTIONARY_PATH2}; // Aggiungi quanti file desideri
std::string line;

std::string rimuoviAccenti(const std::string& input);

int main() {
    auto timer_start = std::chrono::high_resolution_clock::now();
    auto timer_end = timer_start;
    std::chrono::duration<double, std::milli> duration = timer_end - timer_start;
    Dizionario dizionario;

    //---------------------------------------------
    //sezione di creazione del dizionario

    // INSERISCO DIZIONARIO COMUNE
    std::string path = dictionaryPaths[0];
    std::ifstream file(path);

    // Verifica se il file è stato aperto correttamente
    if (!file.is_open()) {
        std::cout << "Errore nell'apertura del dizionario: " << path << std::endl;
        return -1; // Passa al file successivo
    }

    // Legge e inserisce le parole dal file corrente
    while (std::getline(file, line)) {
        dizionario.inserisciParola(line, Labels::DizionarioComune);
    }

    // Chiude il file al termine della lettura
    file.close();

    timer_end = std::chrono::high_resolution_clock::now();
    duration = timer_end - timer_start;
    std::cout << path << " added. Operation time: " << duration.count() << " ms" << std::endl;
    timer_start = timer_end;

    // INSERISCO CONIUGAZIONI
    path = dictionaryPaths[1];
    std::ifstream file2(path);

    // Verifica se il file è stato aperto correttamente
    if (!file2.is_open()) {
        std::cout << "Errore nell'apertura del dizionario: " << path << std::endl;
        return -1; // Passa al file successivo
    }

    // Legge e inserisce le parole dal file corrente
    while (std::getline(file2, line)) {
        dizionario.inserisciParola(line, Labels::Coniugazioni);
    }

    // Chiude il file al termine della lettura
    file2.close();

    timer_end = std::chrono::high_resolution_clock::now();
    duration = timer_end - timer_start;
    std::cout << path << " added. Operation time: " << duration.count() << " ms" << std::endl;
    timer_start = timer_end;

    //---------------------------------------------
    //sezione di check del dizionario

    // Ricerca di parole
    std::string paroleDaCercare[] = {"cane", "casa", "caro", "care", "cat", "cate", "can"};
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