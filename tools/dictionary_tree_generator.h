#ifndef DICTIONARY_TREE_GENERATOR_H
#define DICTIONARY_TREE_GENERATOR_H

#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include <chrono>
#include "../libs/json.hpp"
#include "../libs/dictionary_dir.h"

#define PERCORSO_TREE_DICTIONARY "dizionario.json"

// Definizione dello spazio dei nomi per la libreria json
using json = nlohmann::json;
using String = std::string;

// Definizione della classe Lettera
class Lettera {
public:
    bool fineParola; // Flag per indicare la fine di una parola
    std::unordered_map<char, std::unique_ptr<Lettera>> figli; // Figli dell'oggetto corrente

    // Costruttore
    Lettera() : fineParola(false) {}

    // Metodo per aggiungere una lettera figlia
    Lettera* aggiungiFiglio(char c) {
        if (figli.find(c) == figli.end()) {
            figli[c] = std::make_unique<Lettera>();
        }
        return figli[c].get();
    }

    // Metodo per ottenere un figlio
    Lettera* getFiglio(char c) const {
        auto it = figli.find(c);
        if (it != figli.end()) {
            return it->second.get();
        }
        return nullptr;
    }

    // Funzione ricorsiva per serializzare il nodo e i suoi figli in formato compatto
    json to_json_compatto() const {
        json j;
        j["f"] = fineParola;
        if (!figli.empty()) {
            json figli_json;
            for (const auto& pair : figli) {
                char c = pair.first;
                const std::unique_ptr<Lettera>& figlio = pair.second;
                figli_json[String(1, c)] = figlio->to_json_compatto();
            }
            j["c"] = figli_json;
        }
        return j;
    }

    // Funzione ricorsiva per deserializzare il nodo e i suoi figli dal formato compatto
    static std::unique_ptr<Lettera> from_json_compatto(const json& j) {
        auto nodo = std::make_unique<Lettera>();
        if (j.contains("f")) {
            nodo->fineParola = j["f"].get<bool>();
        }
        if (j.contains("c")) {
            for (auto it = j["c"].begin(); it != j["c"].end(); ++it) {
                char c = it.key()[0];
                nodo->figli[c] = from_json_compatto(it.value());
            }
        }
        return nodo;
    }
};

// Definizione della classe Dizionario
class Dizionario {
private:
    std::unique_ptr<Lettera> radice;

public:
    // Costruttore
    Dizionario() : radice(std::make_unique<Lettera>()) {}

    // Metodo per inserire una parola nel dizionario
    void inserisciParola(const String& parola) {
        Lettera* corrente = radice.get();
        for (char c : parola) {
            corrente = corrente->aggiungiFiglio(c);
        }
        corrente->fineParola = true; // Contrassegna la fine della parola
    }

    // Metodo per cercare una parola nel dizionario
    bool cercaParola(const String& parola) const {
        Lettera* corrente = radice.get();
        for (char c : parola) {
            corrente = corrente->getFiglio(c);
            if (!corrente) {
                return false; // Lettera non trovata
            }
        }
        return corrente->fineParola; // Verifica se è la fine di una parola
    }

    // Metodo per contare il numero di parole nel dizionario
    int contaParole() const {
        return contaParoleRicorsivo(radice.get());
    }

    // Metodo per serializzare il dizionario e salvarlo in un file in formato compatto
    bool salvaInFileCompatto(const String& percorsoFile) const {
        json j = radice->to_json_compatto();
        std::ofstream file(percorsoFile, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Errore nell'aprire il file per la scrittura: " << percorsoFile << std::endl;
            return false;
        }
        file << j.dump(); // Dump senza indentazione per risparmiare spazio
        file.close();
        return true;
    }

    // Metodo per caricare il dizionario da un file JSON compatto
    bool caricaDaFileCompatto(const String& percorsoFile) {
        std::ifstream file(percorsoFile, std::ios::binary);
        if (!file.is_open()) {
            std::cerr << "Errore nell'aprire il file per la lettura: " << percorsoFile << std::endl;
            return false;
        }
        json j;
        file >> j;
        file.close();

        radice = Lettera::from_json_compatto(j);
        return true;
    }

private:
    // Metodo ricorsivo per contare le parole
    int contaParoleRicorsivo(const Lettera* nodo) const {
        int count = nodo->fineParola ? 1 : 0;
        for (const auto& pair : nodo->figli) {
            const std::unique_ptr<Lettera>& figlio = pair.second;
            count += contaParoleRicorsivo(figlio.get());
        }
        return count;
    }
};

#endif
