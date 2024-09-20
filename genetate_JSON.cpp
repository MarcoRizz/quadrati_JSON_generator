#include <iostream>
#include <fstream>
#include <cstdlib>  // Per rand() e srand()
#include <ctime>    // Per time()
#include <chrono>
#include <string>
#include "generate_JSON.h"
#include "libs/json.hpp"
#include "findPath.h"

#define PATH_MAX_STEPS 6

using json = nlohmann::json;

char grid[DIM1][DIM2];
WordList words;
Dizionario dizionario;

void add_element_to_ij(int*& array, int& size, int value);

int main() {
    // Inizializza il seme del generatore di numeri casuali
    auto timer_overall_start = std::chrono::high_resolution_clock::now();
    std::srand(std::time(0));

    /***********************************************************************************
    // CREAZIONE ARRAY-2D GRID
    ***********************************************************************************/
    // Popoliamo grid con lettere casuali
    for (int i = 0; i < DIM1; ++i) {
        for (int j = 0; j < DIM2; ++j) {
            grid[i][j] = 'a' + std::rand() % 26;  // Lettera casuale tra 'a' e 'z' (lavoro con lettere minuscole perché il dizionario usa solo minuscole. Converto in CAPS quando scrivo il JSON)
        }
    }

    //stampo grid[][]
    for (int i = 0; i < DIM1; ++i) {
        for (int j = 0; j < DIM2; ++j) {
            std::cout << String(1, grid[i][j] + 'A' - 'a').c_str() << " ";
        }
        std::cout << std::endl;
    }
    auto timer_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = timer_end - timer_overall_start;
    std::cout << "Array GRID creato - elapsed time: " << duration.count() << " ms" << std::endl;

    /***********************************************************************************
    // CREAZIONE ARRAY-1D WORDS
    ***********************************************************************************/
    //qui devo calcolare l'elenco di parole trovate all'interno della griglia

    //apro il dizionario
    String dictionary_path = (String)"tools\\" + PERCORSO_TREE_DICTIONARY;
    
    if (!dizionario.caricaDaFileCompatto(dictionary_path)) {
        std::cout << "errore nell'apertura del file al percorso: " << dictionary_path << std::endl;
        return -1;
    }
    
    for (int path_size = 4; path_size <= /*DIM1 * DIM2*/PATH_MAX_STEPS; ++path_size) {
        std::cout << "Percorsi con " << path_size << " passi" << std::endl;
        for (int i = 0; i < DIM1; ++i) {
            for (int j = 0; j < DIM2; ++j) {
                findPaths(i, j, 0, path_size);  //qui dentro riempio attemts
            }
        }
    }

    timer_end = std::chrono::high_resolution_clock::now();
    duration = timer_end - timer_overall_start;
    std::cout << "Array WORDS creato, "<< words.get_size() << " valori - elapsed time: " << duration.count() << " ms" << std::endl;

    for (int i = 0; i < words.get_size(); ++i) {
        std::cout <<"Parola #" << i + 1 << ": " << words.get_word(i) << std::endl;
    }
    
    /***********************************************************************************
    // CREAZIONE ARRAY-3D GRID_LINKS
    ***********************************************************************************/
    // la terza dimensione è variabile e archiviata in links_size
    // per mantenere la terza dimensione variabile lavoro coi puntatori
    int*** grid_links = new int**[DIM1];
    int** links_size = new int*[DIM1]; // Array per memorizzare le dimensioni della terza dimensione
    for (int i = 0; i < DIM1; ++i) {
        grid_links[i] = new int*[DIM2];
        links_size[i] = new int[DIM2];
        for (int j = 0; j < DIM2; ++j) {
            // Imposta dimensione variabile per la terza dimensione
            links_size[i][j] = 0; // Memorizza la dimensione della terza dimensione per ogni [i][j]
            grid_links[i][j] = nullptr; // Puntatore inizialmente nullo
        }
    }

    //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (TODO)

    // Popola con valori casuali (TEMP)
    int num_new_elements = 1 + std::rand() % (words.get_size() * DIM1 * DIM2); // Numero casuale di nuovi elementi
    for (int k = 0; k < num_new_elements; ++k) {
        int casual_i = std::rand() % DIM1;
        int casual_j = std::rand() % DIM2;
        add_element_to_ij(grid_links[casual_i][casual_j], links_size[casual_i][casual_j], std::rand() % 10); // Aggiungo elemento a grid_links[casual_i][casual_j]
    }

    //se una lettera rimane priva di link, dovrò sostituirla e ripetere il calcolo (TODO)
    
    /***********************************************************************************
    // Converto tutto in JSON e scrivo il file
    ***********************************************************************************/
    std::cout << "JSON finale:" << std::endl;
    // converto la griglia in JSON
    json grid_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            grid[i][j] += 'A' - 'a'; //trasforma in CAP letter
            std::cout << grid[i][j] << " ";
            row.push_back(String(1, grid[i][j]));  // converto il char in stringa per il JSON
        }
        grid_json.push_back(row);
        std::cout << std::endl;
    }

    // converto le parole in JSON
    json words_json = json::array();
    for (int i = 0; i < words.get_size(); ++i) {
        String word_i = words.get_word(i);
        std::transform(word_i.begin(), word_i.end(), word_i.begin(), ::toupper); // Converte ogni carattere in maiuscolo
        words_json.push_back(word_i);
    }

    // Converti grid_links in JSON
    json grid_links_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json json_row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            json json_link = json::array();
            int num_links = links_size[i][j]; // Ottieni la dimensione della terza dimensione per [i][j]
            for (int k = 0; k < num_links; ++k) {
                json_link.push_back(grid_links[i][j][k]);
            }
            json_row.push_back(json_link);
        }
        grid_links_json.push_back(json_row);
    }


    // creo il contenuto JSON finale
    json data;
    data["grid"] = grid_json;
    data["words"] = words_json;
    data["grid_links"] = grid_links_json;

    // Salvare il JSON in un file
    std::ofstream file("output.json"); //da generare un nome basato sul giorno (TODO)
    if (file.is_open()) {
        file << std::setw(2) << data << std::endl;  // Usa std::setw(2) per una formattazione leggibile
        file.close();
        std::cout << "File JSON generato correttamente!" << std::endl;
    } else {
        std::cerr << "Errore durante l'apertura del file!" << std::endl;
    }

    // Deallocazione della memoria
    for (int i = 0; i < DIM1; ++i) {
        for (int j = 0; j < DIM2; ++j) {
            delete[] grid_links[i][j]; // Dealloca la memoria della terza dimensione
        }
        delete[] grid_links[i]; // Dealloca la memoria della seconda dimensione
        delete[] links_size[i]; // Dealloca la memoria delle dimensioni della terza dimensione
    }
    delete[] grid_links; // Dealloca la memoria della prima dimensione
    delete[] links_size; // Dealloca la memoria delle dimensioni della terza dimensione

    return 0;
}


/***********************************************************************************
// FUNZIONI
***********************************************************************************/

// Funzione per aggiungere un nuovo elemento a un array dinamico (TEMP)
void add_element_to_ij(int*& array, int& size, int value) {
    // Realloca l'array se necessario
    int new_size = size + 1;
    int* new_array = new int[new_size];
    std::memcpy(new_array, array, size * sizeof(int));
    delete[] array;
    array = new_array;
    array[size] = value;
    size = new_size;
}