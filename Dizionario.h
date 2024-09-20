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

    // Metodo per inserire una parola nel dizionario
    void inserisciParola(const String& parola);

    // Metodo per cercare una parola nel dizionario
    bool cercaParola(const String& parola) const;

    // Metodo per contare il numero di parole nel dizionario
    int contaParole() const;

    // Metodo per serializzare il dizionario e salvarlo in un file in formato compatto
    bool salvaInFileCompatto(const String& percorsoFile) const;

    // Metodo per caricare il dizionario da un file JSON compatto
    bool caricaDaFileCompatto(const String& percorsoFile);

private:
    // Metodo ricorsivo per contare le parole
    int contaParoleRicorsivo(const Lettera* nodo) const;
};

#endif