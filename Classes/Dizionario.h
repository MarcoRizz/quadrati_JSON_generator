#ifndef DIZIONARIO_H
#define DIZIONARIO_H

#include <fstream>
#include "Lettera.h"

class Dizionario {
private:
    std::unique_ptr<Lettera> radice;

public:
    // Costruttore
    Dizionario();

    // Metodo per inserire una parola nel dizionario con etichette
    void inserisciParola(const std::string& parola, Labels etichette = Labels::Nessuna);  //chiamare inserisciParola("parola", Labels::DizionarioComune | Labels::BonusAntico) per assegnare più etichette alla parola.

    // Metodo per rimuovere una parola dal dizionario
    bool rimuoviParola(const std::string& parola);

    // Metodo per cercare una parola nel dizionario (opzionalmente con etichetta)
    bool cercaParola(const std::string& parola, Labels etichetta = Labels::Nessuna, bool OR_tra_etichette = false) const;  //cercaParola("parola", Labels::BonusAntico | Labels::BonusPoetico) per filtrare in base alle etichette

    // Metodo per contare il numero di parole nel dizionario
    int contaParole() const;

    // Metodo per contare le parole con una specifica etichetta
    int contaParoleConEtichetta(Labels etichetta, bool OR_tra_etichette = false) const;

    // Metodo per serializzare il dizionario e salvarlo in un file in formato compatto
    bool salvaInFileCompatto(const std::string& percorsoFile) const;

    // Metodo per caricare il dizionario da un file JSON compatto
    bool caricaDaFileCompatto(const std::string& percorsoFile);

private:
    // Metodo ricorsivo per eliminare una parola
    bool rimuoviParolaRicorsivo(Lettera* nodo, const std::string& parola, int indice);

    // Metodo ricorsivo per contare le parole
    int contaParoleRicorsivo(const Lettera* nodo) const;

    // Metodo ricorsivo per contare le parole con etichette
    int contaParoleConEtichettaRicorsivo(const Lettera* nodo, Labels etichetta, bool OR_tra_etichette) const;

    // Metodo per sostituire lettere accentate con lettere UTC-8
    std::string rimuoviAccenti(const std::string& input) const;
};

#endif
