#include <iostream>
#include <fstream>
#include <cstdlib>  // Per rand() e srand()
#include <ctime>    // Per time()
#include <chrono>
#include <string>
#include "generate_JSON.h"
#include "libs/json.hpp"
#include "libs/dictionary_dir.h"
#include "dictionary_ref.h"
#include "findPath.h"
#include "tools/dictionary_tree_generator.h"

using json = nlohmann::json;

char grid[DIM1][DIM2];
int attempts_size = 0;
String* attempts = nullptr;

String generate_random_word(int max_length);
void add_element_to_ij(int*& array, int& size, int value);
int* search_words(String* words, int words_size, const String& dir);
void filter_words(String* words, int* results, int size, String*& filtered_words, int& filtered_size);

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
    int words_size = 0;
    String* words = nullptr;

    //qui devo calcolare l'elenco di parole trovate all'interno della griglia (TODO)
    
    for (int path_size = 4; path_size <= /*DIM1 * DIM2*/5; ++path_size) {
        std::cout << "Percorsi con " << path_size << " passi" << std::endl;
        for (int i = 0; i < DIM1; ++i) {
            for (int j = 0; j < DIM2; ++j) {
                findPaths(i, j, 0, path_size);  //qui dentro compongo attempts[] con add_word()
            }
        }
    }

    timer_end = std::chrono::high_resolution_clock::now();
    duration = timer_end - timer_overall_start;
    std::cout << "Array ATTTEMPTS creato, "<< attempts_size << " valori - elapsed time: " << duration.count() << " ms" << std::endl;

    int* search_results = search_words(attempts, attempts_size, DICTIONARY_PATH);

    timer_end = std::chrono::high_resolution_clock::now();
    duration = timer_end - timer_overall_start;
    std::cout << "Array RESULTS creato - elapsed time: " << duration.count() << " ms" << std::endl;

    filter_words(attempts, search_results, attempts_size, words, words_size);
    
    delete[] search_results;
    delete[] attempts; // Dealloca la memoria dei tentativi

    timer_end = std::chrono::high_resolution_clock::now();
    duration = timer_end - timer_overall_start;
    std::cout << "Array WORDS creato, "<< words_size << " valori rimasti - elapsed time: " << duration.count() << " ms" << std::endl;
    
    // Popoliamo l'array con parole casuali (TEMP)
    /*int num_new_words = 3 + std::rand() % 5; // Numero casuale di parole tra 3 e 7
    for (int i = 0; i < num_new_words; ++i) {
        add_word(words, words_size, generate_random_word(MAX_LENGTH));
    }*/

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
    int num_new_elements = 1 + std::rand() % (words_size * DIM1 * DIM2); // Numero casuale di nuovi elementi
    for (int k = 0; k < num_new_elements; ++k) {
        int casual_i = std::rand() % DIM1;
        int casual_j = std::rand() % DIM2;
        add_element_to_ij(grid_links[casual_i][casual_j], links_size[casual_i][casual_j], std::rand() % 10); // Aggiungo elemento a grid_links[casual_i][casual_j]
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
            grid[i][j] += 'A' - 'a'; //trasforma in CAP letter
            printf("%s ", String(1, grid[i][j]).c_str());
            row.push_back(String(1, grid[i][j]));  // converto il char in stringa per il JSON
        }
        grid_json.push_back(row);
        printf("\n");
    }

    // converto le parole in JSON
    json words_json = json::array();
    for (int i = 0; i < words_size; ++i) {
        std::transform(words[i].begin(), words[i].end(), words[i].begin(), ::toupper); // Converte ogni carattere in maiuscolo
        printf("%s\n", words[i].c_str());
        words_json.push_back(words[i]);
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
    delete[] words; // Dealloca la memoria delle parole
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

// Funzione per generare una parola casuale di lunghezza variabile (TEMP)
String generate_random_word(int max_length) {
    int length = 1 + std::rand() % max_length;  // Lunghezza variabile tra 1 e max_length
    String word;
    for (int i = 0; i < length; ++i) {
        word += 'a' + std::rand() % 26;  // Genera una lettera casuale tra 'a' e 'z'
    }
    return word;
}

// Funzione per aggiungere una nuova stringa in ordine alfabetico a un array dinamico di stringhe
void add_word(String*& array, int& size, const String& word) {
    int new_size = size + 1;
    
    // Trova la posizione in cui inserire la nuova parola
    int insert_pos = 0;
    while (insert_pos < size && array[insert_pos] < word) {
        insert_pos++;
    }

    if (insert_pos < size && array[insert_pos] == word) { //trovata una parola uguale già inserita
        return;
    }

    // Alloca un nuovo array di dimensione aumentata
    String* new_array = new String[new_size];

    // Copia gli elementi prima della posizione di inserimento
    for (int i = 0; i < insert_pos; ++i) {
        new_array[i] = array[i];
    }

    // Inserisce la nuova parola nella posizione corretta
    new_array[insert_pos] = word;

    // Copia gli elementi dopo la posizione di inserimento
    for (int i = insert_pos; i < size; ++i) {
        new_array[i + 1] = array[i];
    }

    // Dealloca la vecchia memoria e aggiorna il puntatore
    delete[] array;
    array = new_array;

    // Aggiorna la dimensione dell'array
    size = new_size;
}


void filter_words(String* words, int* results, int size, String*& filtered_words, int& filtered_size) {
    // Inizializza il nuovo array e la sua dimensione
    filtered_size = 0;
    filtered_words = nullptr;  // Puntatore inizialmente nullo
    
    // Itera su tutte le parole
    for (int i = 0; i < size; ++i) {
        if (results[i] == 1) {
            // Se il risultato è 1, aggiungi la parola all'array filtered_words
            add_word(filtered_words, filtered_size, words[i]);
        }
    }
}


// Funzione per aggiungere un nuovo elemento a un array dinamico
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

// Funzione per cercare parole in un file ordinato e indicizzato
int* search_words(String* words, int words_size, const String& dir) {

    Dizionario dizionario; //classe dizionario creata col tool dictionary_tree_generator
    String dictionary_path = (String)"tools\\" + PERCORSO_TREE_DICTIONARY;

    int* results = new int[words_size]; // Array dinamico per i risultati
    std::memset(results, 0, words_size * sizeof(int)); // Inizializza tutti i risultati a 0

    std::ifstream file(dir);
    
    // Verifica se il file è stato aperto correttamente
    if (!dizionario.caricaDaFileCompatto(dictionary_path)) {
        std::fill(results, results + words_size, -1); // Errore apertura file
        return results;
    }

    String line;
    int w = 0;  // Indice per l'array `words`

    // Loop attraverso ogni parola da cercare
    while (w < words_size) {
        
        bool exact_match_found = false;
        bool contains_match_found = false;

        exact_match_found = dizionario.cercaParola(words[w]);

        // Imposta il risultato appropriato
        if (exact_match_found) {
            results[w] = 1; // Parola esatta trovata
        } else if (contains_match_found) {
            results[w] = 2; // Parola trovata come prefisso
        } else {
            results[w] = 0; // Parola non trovata
        }

        w++; // Passa alla parola successiva
    }

    file.close();
    return results; // Restituisce l'array dei risultati
}