#include "dictionary_tree_generator.h"

int main() {
    auto timer_start = std::chrono::high_resolution_clock::now();
    Dizionario dizionario;

    //---------------------------------------------
    //sezione di creazione del dizionario

    std::ifstream file(DICTIONARY_PATH);
    String line;
    // Verifica se il file è stato aperto correttamente
    if (!file.is_open()) {
        std::cout << "Errore nell'apertura del dizionario." << std::endl; // Errore apertura file
        return 0;
    }

    while (std::getline(file, line)) {
        dizionario.inserisciParola(line);
    }
    
    auto timer_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = timer_end - timer_start;
    std::cout << "Elapsed time: " << duration.count() << " ms" << std::endl;
    
    /*// Inserimento di parole nel dizionario
    dizionario.inserisciParola("cane");
    dizionario.inserisciParola("casa");
    dizionario.inserisciParola("caro");
    dizionario.inserisciParola("care");
    dizionario.inserisciParola("cat");
    dizionario.inserisciParola("cane"); // Parola duplicata*/

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
