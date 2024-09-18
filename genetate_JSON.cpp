#include <iostream>
#include <fstream>
#include <cstdlib>  // Per rand() e srand()
#include <ctime>    // Per time()
#include <vector>   // Per std::vector
#include <string>
#include "generate_JSON.h"

int main() {
    // Inizializza il seme del generatore di numeri casuali
    std::srand(std::time(0));

    // Creiamo un array 2D normale per la griglia
    char grid[4][4];

    // Popoliamo grid con lettere casuali
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            grid[i][j] = 'A' + std::rand() % 26;  // Lettera casuale tra 'A' e 'Z'
        }
    }

    //qui devo calcolare l'elenco di parole trovate all'interno della griglia (TODO)
    // Lavoriamo con un vettore di parole (normale array C++)
    std::vector<String> words = {"stocazzo", "pippo", "giovanni", "francesco", "gianfrancesco"};

    //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (TODO)
    // Lavoriamo con un vettore 2D di vettori per grid_links
    std::vector<std::vector<std::vector<int>>> grid_links = {
        {{0, 4}, {0, 1, 2, 3, 4}, {0}, {3}},
        {{0}, {0, 1, 2}, {0}, {2, 3}},
        {{4}, {4}, {}, {1}},
        {{3, 4}, {0, 4}, {0}, {1}}
    };

    //se una lettera rimane priva di link, dovrò sostituirla e ripetere il calcolo (TODO)

    // Alla fine, converto tutto in JSON prima della scrittura

    // converto la griglia in JSON
    json grid_json = json::array();
    for (int i = 0; i < 4; ++i) {
        json row = json::array();
        for (int j = 0; j < 4; ++j) {
            row.push_back(String(1, grid[i][j]));  // converto il char in stringa per il JSON
        }
        grid_json.push_back(row);
    }

    // converto le parole in JSON
    json words_json = json::array();
    for (const auto& word : words) {
        words_json.push_back(word);
    }

    // converto i grid_links in JSON
    json grid_links_json = json::array();
    for (const auto& row : grid_links) {
        json json_row = json::array();
        for (const auto& link : row) {
            json_row.push_back(link);
        }
        grid_links_json.push_back(json_row);
    }

    // creo il contenuto JSON finale
    json data;
    data["grid"] = grid_json;
    data["words"] = words_json;
    data["grid_links"] = grid_links_json;

    // Salvare il JSON in un file
    std::ofstream file("output.json");
    if (file.is_open()) {
        file << std::setw(2) << data << std::endl;  // Usa std::setw(2) per una formattazione leggibile
        file.close();
        std::cout << "File JSON generato correttamente!" << std::endl;
    } else {
        std::cerr << "Errore durante l'apertura del file!" << std::endl;
    }

    return 0;
}
