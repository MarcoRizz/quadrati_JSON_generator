#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <fstream>
#include <chrono>
#include "../libs/json.hpp"
#include "../libs/dictionary_dir.h"

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
                figli_json[std::string(1, c)] = figlio->to_json_compatto();
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
    void inserisciParola(const std::string& parola) {
        Lettera* corrente = radice.get();
        for (char c : parola) {
            corrente = corrente->aggiungiFiglio(c);
        }
        corrente->fineParola = true; // Contrassegna la fine della parola
    }

    // Metodo per cercare una parola nel dizionario
    bool cercaParola(const std::string& parola) const {
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
    bool salvaInFileCompatto(const std::string& percorsoFile) const {
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
    bool caricaDaFileCompatto(const std::string& percorsoFile) {
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

int main() {
    auto timer_overall_start = std::chrono::high_resolution_clock::now();
    Dizionario dizionario;

    //---------------------------------------------
    //sezione di creazione del dizionario

    int max_words = 1000000;
    std::ifstream file(DICTIONARY_PATH);
    String line;
    // Verifica se il file è stato aperto correttamente
    if (!file.is_open()) {
        std::cout << "Errore nell'apertura del dizionario." << std::endl; // Errore apertura file
        return 0;
    }

    int word_i = 0;
    while (word_i < max_words) {
        if (!std::getline(file, line)) {
            break; // Esci se non ci sono più righe
        }
        dizionario.inserisciParola(line);
        word_i++;
    }
    
    auto timer_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = timer_end - timer_overall_start;
    std::cout << "Elapsed time: " << duration.count() << " ms" << std::endl;

    /*// Inserimento di parole nel dizionario
    dizionario.inserisciParola("cane");
    dizionario.inserisciParola("casa");
    dizionario.inserisciParola("caro");
    dizionario.inserisciParola("care");
    dizionario.inserisciParola("cat");
    dizionario.inserisciParola("cane"); // Parola duplicata*/

    // Ricerca di parole
    std::string paroleDaCercare[] = {"cane", "casa", "caro", "care", "cat", "cate", "can"};
    for (const auto& parola : paroleDaCercare) {
        if (dizionario.cercaParola(parola)) {
            std::cout << "La parola \"" << parola << "\" esiste nel dizionario." << std::endl;
        } else {
            std::cout << "La parola \"" << parola << "\" non esiste nel dizionario." << std::endl;
        }
    }

    // Conteggio delle parole nel dizionario
    int totaleParole = dizionario.contaParole();
    std::cout << "\nNumero totale di parole nel dizionario: " << totaleParole << std::endl;

    // Serializzazione del dizionario
    std::string percorsoFile = "dizionario.json";
    if (dizionario.salvaInFileCompatto(percorsoFile)) {
        std::cout << "Dizionario salvato correttamente in \"" << percorsoFile << "\"." << std::endl;
    }

    // Creazione di un nuovo dizionario e deserializzazione da file
    Dizionario nuovoDizionario;
    if (nuovoDizionario.caricaDaFileCompatto(percorsoFile)) {
        std::cout << "Dizionario caricato correttamente da \"" << percorsoFile << "\"." << std::endl;
    }

    // Verifica delle parole nel nuovo dizionario
    for (const auto& parola : paroleDaCercare) {
        if (nuovoDizionario.cercaParola(parola)) {
            std::cout << "La parola \"" << parola << "\" esiste nel nuovo dizionario." << std::endl;
        } else {
            std::cout << "La parola \"" << parola << "\" non esiste nel nuovo dizionario." << std::endl;
        }
    }

    // Conteggio delle parole nel nuovo dizionario
    int totaleParoleNuovo = nuovoDizionario.contaParole();
    std::cout << "\nNumero totale di parole nel nuovo dizionario: " << totaleParoleNuovo << std::endl;

    return 0;
}
