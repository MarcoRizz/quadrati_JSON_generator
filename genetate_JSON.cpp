#include <iostream>
#include <fstream>
#include <cstdlib>  // Per rand() e srand()
#include <ctime>    // Per time()
#include "libs/json.hpp"

using json = nlohmann::json;

int main() {
    // Inizializza il seme del generatore di numeri casuali
    std::srand(std::time(0));

    // Creiamo il contenuto JSON
    json data;

    // Generazione della griglia di lettere casuali 4x4
    json grid = json::array();
    for (int i = 0; i < 4; ++i) {
        json row = json::array();
        for (int j = 0; j < 4; ++j) {
            // Genera una lettera casuale tra 'A' e 'Z'
            char random_letter = 'A' + std::rand() % 26;
            row.push_back(std::string(1, random_letter));
        }
        grid.push_back(row);
    }

    //qui devo calcolare l'elenco di parole trovate all'interno della griglia (TODO)
    json words = {"stocazzo", "pippo", "giovanni", "francesco", "gianfrancesco"};

    //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (TODO)
    json grid_links = {
        {{0, 4}, {0, 1, 2, 3, 4}, {0}, {3}},
        {{0}, {0, 1, 2}, {0}, {2, 3}},
        {{4}, {4}, {}, {1}},
        {{3, 4}, {0, 4}, {0}, {1}}
    };

    //se una lettera rimane priva di link devo sostituirla con una nuova lettera e ripetere tutto il processo di calcolo (TODO)

    // Riempire la sezione
    data["grid"] = grid;
    data["words"] = words;
    data["grid_links"] = grid_links;

    // Salvare il JSON in un file
    std::ofstream file("output.json");
    if (file.is_open()) {
        file << std::setw(2) << data << std::endl; // Usa std::setw(2) per formattare con indentazione
        file.close();
        std::cout << "File JSON generato correttamente!" << std::endl;
    } else {
        std::cerr << "Errore durante l'apertura del file!" << std::endl;
    }

    return 0;
}
