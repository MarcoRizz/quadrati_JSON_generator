#ifndef LETTERA_H
#define LETETRA_H

#include <iostream>
#include <unordered_map>
#include "../libs/json.hpp"

#define PERCORSO_TREE_DICTIONARY "dizionario.json"

// Definizione dello spazio dei nomi per la libreria json
using json = nlohmann::json;
using String = std::string;

class Lettera {
public:
    bool fineParola; // Flag per indicare la fine di una parola
    std::unordered_map<char, std::unique_ptr<Lettera>> figli; // Figli dell'oggetto corrente

    // Costruttore
    Lettera();

    // Metodo per aggiungere una lettera figlia
    Lettera* aggiungiFiglio(char c);

    // Metodo per rimuovere una lettera figlia
    bool Lettera::rimuoviFiglio(char c);

    // Metodo per ottenere un figlio
    Lettera* getFiglio(char c) const;

    // Funzione ricorsiva per serializzare il nodo e i suoi figli in formato compatto
    json to_json_compatto() const;

    // Funzione ricorsiva per deserializzare il nodo e i suoi figli dal formato compatto
    static std::unique_ptr<Lettera> from_json_compatto(const json& j);
};

#endif