#ifndef LETTERA_H
#define LETTERA_H

#include <unordered_map>
#include <memory>
#include <cstdint>
#include "nlohmann_json.cpp"
#include "etichette.h"

// Definizione dello spazio dei nomi per la libreria json
using json = nlohmann::json;

class Lettera {
public:
    bool fineParola; // Flag per indicare la fine di una parola
    std::unordered_map<char, std::unique_ptr<Lettera>> figli; // Figli dell'oggetto corrente
    uint32_t etichette; // Etichette combinate (bitmask)

    // Costruttore
    Lettera();

    // Metodo per aggiungere una lettera figlia
    Lettera* aggiungiFiglio(char c);

    // Metodo per rimuovere una lettera figlia
    bool rimuoviFiglio(char c);

    // Metodo per ottenere un figlio
    Lettera* getFiglio(char c) const;

    // Metodi per gestire le etichette
    void aggiungiEtichetta(Labels label);
    void rimuoviEtichetta(Labels label);
    bool haEtichetta(Labels label) const;

    // Funzione ricorsiva per serializzare il nodo e i suoi figli in formato compatto
    json to_json_compatto() const;

    // Funzione ricorsiva per deserializzare il nodo e i suoi figli dal formato compatto
    static std::unique_ptr<Lettera> from_json_compatto(const json& j);
};

#endif
 // LETTERA_H
