#include "lettera.h"

#include <iostream>


// Metodi standard
Lettera* Lettera::aggiungiFiglio(char c) {
    if (figli.find(c) == figli.end()) {
        figli[c] = std::make_unique<Lettera>();
    }
    return figli[c].get();
}

bool Lettera::rimuoviFiglio(char c) {
    auto it = figli.find(c);
    if (it != figli.end()) {
        figli.erase(it);
        return true;
    }
    return false;
}

Lettera* Lettera::getFiglio(char c) const {
    auto it = figli.find(c);
    if (it != figli.end()) {
        return it->second.get();
    }
    return nullptr;
}

// Metodi per gestire le etichette
void Lettera::aggiungiEtichetta(Etichette label) {
    etichette.aggiungiEtichetta(label); // Aggiunge l'etichetta
}

void Lettera::rimuoviEtichetta(Etichette label) {
    etichette.rimuoviEtichetta(label); // Rimuove l'etichetta
}

// Serializzazione in JSON
json Lettera::to_json_compatto() const {
    json j;
    j["FP"] = (uint32_t)fineParola; // Flag di fine parola
    if (fineParola) {
        j["LB"] = etichette.printInt(); // Salva le etichette come intero
    }

    // Aggiungi i figli come chiavi al nodo JSON principale, senza il livello "c"
    for (const auto& pair : figli) {
        char c = pair.first;
        const std::unique_ptr<Lettera>& figlio = pair.second;

        // Verifica che 'c' sia un carattere stampabile
        if (c < 32 || c > 126) {
            std::cerr << "Carattere non valido per JSON: " << static_cast<int>(c) << std::endl;
            continue; // Salta il carattere se non è valido
        }

        // Aggiungi il figlio direttamente come chiave, ricorsivamente
        j[std::string(1, c)] = figlio->to_json_compatto();
    }

    return j;
}


// Deserializzazione dal JSON
std::unique_ptr<Lettera> Lettera::from_json_compatto(const json& j) {
    auto nodo = std::make_unique<Lettera>();

    // Leggi il flag di fine parola, se presente
    if (j.contains("FP")) {
        nodo->fineParola = (bool)j["FP"].get<uint32_t>();
        if (j.contains("LB")) {
            nodo->setEtichette(j["LB"].get<uint32_t>()); // Carica le etichette come intero
        }
    }

    // Itera attraverso i membri JSON per recuperare i figli
    for (auto it = j.begin(); it != j.end(); ++it) {
        if (it.key() == "FP" || it.key() == "LB") continue; // Salta il flag di fine parola

        char c = it.key()[0]; // Ottieni il carattere per il figlio
        nodo->figli[c] = from_json_compatto(it.value()); // Ricorsivamente deserializza il figlio
    }

    return nodo;
}

