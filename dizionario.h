#ifndef DIZIONARIO_H
#define DIZIONARIO_H

#include <optional>
#include "lettera.h"

class Dizionario {
private:
    std::unique_ptr<Lettera> radice;

public:
    // Costruttore
    Dizionario();

    // Metodo per inserire una parola nel dizionario con etichette
    void inserisciParola(const std::string& parola, Etichette etichette = Etichette(Etichette::Nessuna), bool sovrascriviEtichette = false);  //chiamare inserisciParola("parola", Etichette::DizionarioComune | Etichette::BonusAntico) per assegnare più etichette alla parola.

    // Metodo per rimuovere una parola dal dizionario
    bool rimuoviParola(const std::string& parola);

    // Metodo per cercare una parola nel dizionario (opzionalmente con etichetta)
    std::optional<Etichette> cercaParola(const std::string& parola, Etichette etichette = Etichette(Etichette::Tutte), bool OR_tra_etichette = false) const;  //cercaParola("parola", Etichette::BonusAntico | Etichette::BonusPoetico) per filtrare in base alle etichette

    // Metodo per contare il numero di parole nel dizionario
    int contaParole() const;

    // Metodo per contare le parole con una specifica etichetta
    int contaParoleConEtichetta(Etichette etichette, bool OR_tra_etichette = false) const;

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
    int contaParoleConEtichettaRicorsivo(const Lettera* nodo, Etichette etichette, bool OR_tra_etichette) const;

    // Metodo per sostituire lettere accentate con lettere UTC-8
    std::string rimuoviAccenti(const std::string& input) const;
};

#endif // DIZIONARIO_H
