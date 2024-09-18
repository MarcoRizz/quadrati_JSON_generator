#include <iostream>
#include <fstream>
#include "libs/json.hpp"

using json = nlohmann::json;

int main() {
    // Creiamo il contenuto JSON
    json data;

    // Dati per "today" e "yesterday"
    json grid = {
        {"A", "B", "C", "D"},
        {"E", "F", "G", "H"},
        {"I", "J", "K", "L"},
        {"M", "N", "O", "P"}
    };

    json grid_links = {
        {{0, 4}, {0, 1, 2, 3, 4}, {0}, {3}},
        {{0}, {0, 1, 2}, {0}, {2, 3}},
        {{4}, {4}, {}, {1}},
        {{3, 4}, {0, 4}, {0}, {1}}
    };

    json words = {"stocazzo", "pippo", "giovanni", "francesco", "gianfrancesco"};

    // Riempire la sezione "today"
    data["today"]["grid"] = grid;
    data["today"]["grid_links"] = grid_links;
    data["today"]["words"] = words;

    // Riempire la sezione "yesterday" (stessa struttura di "today")
    data["yesterday"]["grid"] = grid;
    data["yesterday"]["grid_links"] = grid_links;
    data["yesterday"]["words"] = words;

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
