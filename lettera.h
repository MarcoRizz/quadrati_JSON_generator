#ifndef LETTERA_H
#define LETTERA_H

#include <unordered_map>
#include <memory>
#include "nlohmann_json.cpp"
#include "etichette.h"

// Definizione dello spazio dei nomi per la libreria json
using json = nlohmann::json;

class Lettera {
public:
    bool fineParola; // Flag per indicare la fine di una parola
    std::unordered_map<char, std::unique_ptr<Lettera>> figli; // Figli dell'oggetto corrente
    Etichette etichette; // Etichette combinate (bitmask)

    // Costruttore che accetta un carattere e un oggetto Etichette
    Lettera(const Etichette& et = Etichette()) : fineParola(false), etichette(et) {}

    // Costruttore che accetta un carattere e un valore numerico delle etichette
    Lettera(Etichette::TipoEtichetta et_val) : fineParola(false), etichette(et_val) {}

    // Metodo per aggiungere una lettera figlia
    Lettera* aggiungiFiglio(char c);

    // Metodo per rimuovere una lettera figlia
    bool rimuoviFiglio(char c);

    // Metodo per ottenere un figlio
    Lettera* getFiglio(char c) const;

    // Metodo per assegnare una nuova etichetta
    void setEtichette(const Etichette& et) { etichette = et; }

    // Metodo sovraccaricato per assegnare un valore uint32_t
    void setEtichette(uint32_t et_val) { etichette = Etichette(et_val); }

    // Metodi per gestire le etichette
    void aggiungiEtichetta(Etichette label);
    void rimuoviEtichetta(Etichette label);

    // Funzione ricorsiva per serializzare il nodo e i suoi figli in formato compatto
    json to_json_compatto() const;

    // Funzione ricorsiva per deserializzare il nodo e i suoi figli dal formato compatto
    static std::unique_ptr<Lettera> from_json_compatto(const json& j);
};

#endif
 // LETTERA_H
