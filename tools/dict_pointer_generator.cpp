#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstddef> // Per std::size_t
#include "..\libs\dictionary_dir.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>

#define ALPHABET_SIZE 26

// Funzione per generare gli indici di inizio delle righe per prefissi fino a 4 lettere
void generate_prefix_indices(const std::string& file_path, std::map<std::string, std::streampos>& indices) {
    std::ifstream file(file_path, std::ios::in | std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Errore nell'apertura del file!" << std::endl;
        return;
    }

    std::string line;
    std::streampos current_position = 0;
    std::string prefix;

    while (std::getline(file, line)) {
        current_position = file.tellg(); // Ottieni la posizione corrente nel file
        if (!line.empty()) {
            // Usa solo le prime 4 lettere della riga come prefisso
            for (std::size_t i = 0; i < line.size() && i < 4; ++i) {
                prefix += line[i];
                if (indices.find(prefix) == indices.end()) {
                    indices[prefix] = current_position;
                }
            }
            // Azzera il prefisso per la prossima riga
            prefix.clear();
        }
    }

    file.close();
}

int main() {
    // Mappa per memorizzare le posizioni degli indici dei prefissi
    std::map<std::string, std::streampos> prefix_indices;
    generate_prefix_indices(DICTIONARY_PATH, prefix_indices);

    // Stampa le posizioni degli indici per i prefissi
    for (const auto& entry : prefix_indices) {
        std::cout << "Prefisso: " << entry.first << " - Posizione: " << entry.second << std::endl;
    }

    return 0;
}
