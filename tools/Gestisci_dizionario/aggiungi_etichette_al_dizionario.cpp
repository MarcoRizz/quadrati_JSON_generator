#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include "../../Classes/Dizionario.h"

#define PERCORSO_TREE_DICTIONARY "..\\Dizionari\\dizionario.json"

typedef struct {
    std::string parola;
    Labels label;
} mod_dizionario;

bool soloSePresenti = true;

// Inizializzazione di un array di std::string
std::vector<mod_dizionario> modsDaElaborare = {
    {"disdico", Labels::Approvate | Labels::Coniugazioni}
    };

// Inizializzazione di un array di std::string
std::vector<mod_dizionario> modsSoloSePresenti = {
    {"unicef", Labels::BonusNome}
    };

Dizionario dizionario;

int main() {
     //apro il dizionario
    if (!dizionario.caricaDaFileCompatto(PERCORSO_TREE_DICTIONARY)) {
        std::cout << "errore nell'apertura del file al percorso: " << PERCORSO_TREE_DICTIONARY << std::endl;
        return -1;
    }
    
    // azione
    for (const auto& mod : modsDaElaborare) {
        std::string parola = mod.parola;

        dizionario.inserisciParola(parola, mod.label);
        std::cout << parola << " elaborata" << std::endl;
    }

    for (const auto& mod : modsSoloSePresenti) {
        std::string parola = mod.parola;

        //AGGIUNGI ETICHETTA
        if (!dizionario.cercaParola(parola)) {
            std::cout << parola << " non trovata" << std::endl;
            continue;
        }
        dizionario.inserisciParola(parola, mod.label);
        std::cout << parola << " elaborata" << std::endl;
    }

    std::cout << "Parole presenti: " << dizionario.contaParole() << std::endl;
    dizionario.salvaInFileCompatto(PERCORSO_TREE_DICTIONARY);

    std::cout << "Parole modificate correttamente" << std::endl;
}