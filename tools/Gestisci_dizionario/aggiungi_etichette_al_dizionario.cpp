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
    {"deco", Labels::BonusNome}
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

        //AGGIUNGI ETICHETTA
        /*if (soloSePresenti && !dizionario.cercaParola(parola)) {
            std::cout << parola << " non trovata" << std::endl;
            continue;
        }
        dizionario.inserisciParola(parola, mod.label);
        std::cout << parola << " elaborata" << std::endl;*/

        //LEGGI ETICHETTE
        std::cout << parola << ":" << std::endl;
        std::cout << "Senza etichette: " << dizionario.cercaParola(parola) << std::endl;
        std::cout << "DizionarioComune: " << dizionario.cercaParola(parola, Labels::DizionarioComune) << std::endl;
        std::cout << "Coniugazioni: " << dizionario.cercaParola(parola, Labels::Coniugazioni) << std::endl;
        std::cout << "Approvate: " << dizionario.cercaParola(parola, Labels::Approvate) << std::endl;
        std::cout << "BonusNome: " << dizionario.cercaParola(parola, Labels::BonusNome) << std::endl;
        std::cout << "BonusRaro: " << dizionario.cercaParola(parola, Labels::BonusRaro) << std::endl;
        std::cout << "BonusStraniero: " << dizionario.cercaParola(parola, Labels::BonusStraniero) << std::endl;
    }

    dizionario.salvaInFileCompatto(PERCORSO_TREE_DICTIONARY);

    std::cout << "Parole modificate correttamente" << std::endl;
}