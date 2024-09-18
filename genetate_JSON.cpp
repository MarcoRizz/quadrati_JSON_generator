#include <iostream>
#include <fstream>
#include <cstdlib>  // Per rand() e srand()
#include <ctime>    // Per time()
#include <vector>   // Per std::vector
#include <string>
#include "generate_JSON.h"

const int DIM1 = 4;
const int DIM2 = 4;
const int MAX_LENGTH = 16; // Lunghezza massima delle parole
int MAX_WORDS = 10;  // Numero massimo di parole  //sarà da sostituire col numero di parole trovate (TODO)

String generate_random_word(int max_length);
void add_element(int*& array, int& size, int value);

int main() {
    // Inizializza il seme del generatore di numeri casuali
    std::srand(std::time(0));

    /***********************************************************************************
    // CREAZIONE ARRAY-2D GRID
    ***********************************************************************************/
    char grid[DIM1][DIM2];

    // Popoliamo grid con lettere casuali
    for (int i = 0; i < DIM1; ++i) {
        for (int j = 0; j < DIM2; ++j) {
            grid[i][j] = 'A' + std::rand() % 26;  // Lettera casuale tra 'A' e 'Z'
        }
    }

    //qui devo calcolare l'elenco di parole trovate all'interno della griglia (TODO)

    /***********************************************************************************
    // CREAZIONE ARRAY-1D WORDS
    ***********************************************************************************/
    String words[MAX_WORDS];

    // Popoliamo l'array con parole casuali (TEMP)
    for (int i = 0; i < MAX_WORDS; ++i) {
        words[i] = generate_random_word(MAX_LENGTH);
    }

    //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (TODO)

    /***********************************************************************************
    // CREAZIONE ARRAY-3D GRID_LINKS
    ***********************************************************************************/
    // la terza dimensione è variabile e archiviata in link_sizes
    // per mantenere la terza dimensione variabile lavoro coi puntatori
    int*** grid_links = new int**[DIM1];
    int** link_sizes = new int*[DIM1]; // Array per memorizzare le dimensioni della terza dimensione
    for (int i = 0; i < DIM1; ++i) {
        grid_links[i] = new int*[DIM2];
        link_sizes[i] = new int[DIM2];
        for (int j = 0; j < DIM2; ++j) {
            // Imposta dimensione variabile per la terza dimensione
            link_sizes[i][j] = 0; // Memorizza la dimensione della terza dimensione per ogni [i][j]
            grid_links[i][j] = nullptr; // Puntatore inizialmente nullo
        }
    }

    //assegno i link tra lettere e parole
    // Popola con valori casuali (TEMP)
    int num_new_elements = 1 + std::rand() % (MAX_WORDS * DIM1 * DIM2); // Numero casuale di nuovi elementi
    for (int k = 0; k < num_new_elements; ++k) {
        int casual_i = std::rand() % DIM1;
        int casual_j = std::rand() % DIM2;
        add_element(grid_links[casual_i][casual_j], link_sizes[casual_i][casual_j], std::rand() % 10); // Aggiungo elemento a grid_links[casual_i][casual_j]
    }

    //se una lettera rimane priva di link, dovrò sostituirla e ripetere il calcolo (TODO)
    
    /***********************************************************************************
    // Converto tutto in JSON e scrivo il file
    ***********************************************************************************/
    // converto la griglia in JSON
    json grid_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            row.push_back(String(1, grid[i][j]));  // converto il char in stringa per il JSON
        }
        grid_json.push_back(row);
    }

    // converto le parole in JSON
    json words_json = json::array();
    for (const auto& word : words) {
        words_json.push_back(word);
    }

    // Converti grid_links in JSON
    json grid_links_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json json_row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            json json_link = json::array();
            int num_links = link_sizes[i][j]; // Ottieni la dimensione della terza dimensione per [i][j]
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
    std::ofstream file("output.json");
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
        delete[] link_sizes[i]; // Dealloca la memoria delle dimensioni della terza dimensione
    }
    delete[] grid_links; // Dealloca la memoria della prima dimensione
    delete[] link_sizes; // Dealloca la memoria delle dimensioni della terza dimensione

    return 0;
}

// Funzione per generare una parola casuale di lunghezza variabile (TEMP)
String generate_random_word(int max_length) {
    int length = 1 + std::rand() % max_length;  // Lunghezza variabile tra 1 e max_length
    String word;
    for (int i = 0; i < length; ++i) {
        word += 'a' + std::rand() % 26;  // Genera una lettera casuale tra 'a' e 'z'
    }
    return word;
}

// Funzione per aggiungere un nuovo elemento a un array dinamico
void add_element(int*& array, int& size, int value) {
    // Realloca l'array se necessario
    int new_size = size + 1;
    int* new_array = new int[new_size];
    std::memcpy(new_array, array, size * sizeof(int));
    delete[] array;
    array = new_array;
    array[size] = value;
    size = new_size;
}