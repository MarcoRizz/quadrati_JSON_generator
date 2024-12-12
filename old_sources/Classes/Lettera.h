#ifndef LETTERA_H
#define LETTERA_H

#include <iostream>
#include <unordered_map>
#include <memory>
#include <cstdint>
#include "../libs/json.hpp"

// Definizione dello spazio dei nomi per la libreria json
using json = nlohmann::json;

// Enum per le etichette (bitmasking)
enum Labels : uint32_t {
    Nessuna          = 0,       // 000000
    DizionarioComune = 1u << 0, // 000001
    Coniugazioni     = 1u << 1, // 000010
    Approvate        = 1u << 2, // 000100
    BonusRaro        = 1u << 3, // 001000
    BonusStraniero   = 1u << 4, // 010000
    BonusNome        = 1u << 5  // 100000
};

// Definizione degli operatori di bitmask per Labels
inline Labels operator|(Labels a, Labels b) {
    return static_cast<Labels>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline Labels operator&(Labels a, Labels b) {
    return static_cast<Labels>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

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
