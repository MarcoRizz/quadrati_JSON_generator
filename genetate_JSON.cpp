#include <iostream>
#include <fstream>
#include <cstdlib>  // Per rand() e srand()
#include <ctime>    // Per time()
#include <chrono>
#include <string>
#include "generate_JSON.h"
#include "libs/json.hpp"
#include "findPath.h"

#define SERIAL_NUMBER 1

#define PATH_MAX_STEPS 10
#define MAX_LOOPS 10

using json = nlohmann::json;

char grid[DIM1][DIM2];
WordList words;
Dizionario dizionario;
std::vector<std::vector<DynArray>> passingWords(DIM1, std::vector<DynArray>(DIM2, DynArray(words.get_size())));
std::vector<std::vector<DynArray>> startingWords(DIM1, std::vector<DynArray>(DIM2, DynArray(words.get_size())));

int main() {
    // Inizializza il seme del generatore di numeri casuali
    auto timer_overall_start = std::chrono::high_resolution_clock::now();
    std::srand(std::time(0));

    //apro il dizionario
    String dictionary_path = (String)"tools\\" + PERCORSO_TREE_DICTIONARY;
    
    if (!dizionario.caricaDaFileCompatto(dictionary_path)) {
        std::cout << "errore nell'apertura del file al percorso: " << dictionary_path << std::endl;
        return -1;
    }
    auto timer_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = timer_end - timer_overall_start;
    std::cout << "Dizionario caricato - elapsed time: " << duration.count() << " ms" << std::endl;
    
    bool completed_grid = false;
    int loop = 0;

    while (!completed_grid  && loop < MAX_LOOPS) {
        /***********************************************************************************
        // CREAZIONE ARRAY-2D GRID
        ***********************************************************************************/
        // Popoliamo grid con lettere casuali
        for (int i = 0; i < DIM1; ++i) {
            for (int j = 0; j < DIM2; ++j) {
                if (passingWords[i][j].get_size() == 0) {
                    grid[i][j] = 'a' + std::rand() % 26;  // Lettera casuale tra 'a' e 'z' (lavoro con lettere minuscole perché il dizionario usa solo minuscole. Converto in CAPS quando scrivo il JSON)
                }
            }
        }
        
        //stampo grid[][]
        for (int j = 0; j < DIM2; ++j) {
            for (int i = 0; i < DIM1; ++i) {
                std::cout << String(1, grid[i][j] + 'A' - 'a').c_str() << " ";
            }
            std::cout << std::endl;
        }
        timer_end = std::chrono::high_resolution_clock::now();
        duration = timer_end - timer_overall_start;
        std::cout << "Array GRID creato - elapsed time: " << duration.count() << " ms" << std::endl;

        /***********************************************************************************
        // CREAZIONE ARRAY-1D WORDS
        ***********************************************************************************/
        //qui devo calcolare l'elenco di parole trovate all'interno della griglia

        #ifdef PATH_MAX_STEPS
        std::cout << "Numero massimo di passi impostato: " << PATH_MAX_STEPS << std::endl;
        #endif
        for (int path_size = 4; path_size <= /*DIM1 * DIM2*/PATH_MAX_STEPS; ++path_size) {
            for (int i = 0; i < DIM1; ++i) {
                for (int j = 0; j < DIM2; ++j) {
                    findPaths(i, j, 0, path_size);  //qui dentro riempio words
                }
            }
        }

        timer_end = std::chrono::high_resolution_clock::now();
        duration = timer_end - timer_overall_start;
        std::cout << "Array WORDS: "<< words.get_size() << " valori totali - elapsed time: " << duration.count() << " ms" << std::endl;
        
        /***********************************************************************************
        // CREAZIONE ARRAY-3D GRID_LINKS
        ***********************************************************************************/
        //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (e quali possono iniziare)

        for (int word_i = 0; word_i < words.get_size(); ++word_i) {
            String running_word = words.get_word(word_i);
            for (int i = 0; i < DIM1; ++i) {
                for (int j = 0; j < DIM2; ++j) {
                    if(grid[i][j] == running_word[0]) {
                        findWordPaths(i, j, 0, running_word, word_i);  //qui dentro riempio passingWords e startingWords
                    }
                }
            }
        }

        //se una lettera rimane priva di link, dovrò sostituirla e ripetere il calcolo (TODO)
        completed_grid = true;
        for (int i = 0; i < DIM1; ++i) {
            for (int j = 0; j < DIM2; ++j) {
                if (passingWords[i][j].get_size() == 0) {
                    completed_grid = false;
                    break;
                }
            }
            if (!completed_grid) {
                std::cout << "Elaboro una nuova griglia sostituendo le lettere inutilizzate (iterazione " << loop + 1 << ")" << std::endl << std::endl;
                break;
            }
        }
        loop++;
    }

    if (loop == MAX_LOOPS) {
        std::cerr << "Numero massimo di iterazioni raggiunto, non è stata trovata una griglia" << std::endl;
        return -1;
    } else {
        std::cout << "Loops: " << loop << std::endl;
    }

    /***********************************************************************************
    // Converto tutto in JSON e scrivo il file
    ***********************************************************************************/
    std::cout << "------------" << std::endl << "JSON FINALE:" << std::endl << std::endl << "grid:" << std::endl;
    // converto la griglia in JSON
    json grid_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            grid[i][j] += 'A' - 'a'; //trasforma in CAP letter
            row.push_back(String(1, grid[i][j]));  // converto il char in stringa per il JSON
        }
        grid_json.push_back(row);
    }
    //stampo grid[][]
    for (int j = 0; j < DIM2; ++j) {
        for (int i = 0; i < DIM1; ++i) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << std::endl;
    }

    std::cout << std::endl << "words:" << std::endl;
    // converto le parole in JSON
    json words_json = json::array();
    for (int i = 0; i < words.get_size(); ++i) {
        String word_i = words.get_word(i);
        std::cout << "#" << i << ":" << word_i << std::endl;
        std::transform(word_i.begin(), word_i.end(), word_i.begin(), ::toupper); // Converte ogni carattere in maiuscolo
        words_json.push_back(word_i);
    }

    // Converti grid_links in JSON
    std::cout << std::endl << "words passingLinks:" << std::endl;
    json grid_passingLinks_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json json_row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            json json_link = json::array();
            int num_links = passingWords[i][j].get_size(); // Ottieni il numero di collegamenti

            std::cout << "[" << i << "][" << j << "]: ";
            for (int k = 0; k < num_links; ++k) {
                int value = passingWords[i][j].get_value(k);
                std::cout << value << " ";
                json_link.push_back(value);
            }
            json_row.push_back(json_link);
            std::cout << std::endl;
        }
        grid_passingLinks_json.push_back(json_row);
    }

    // Converti grid_links in JSON
    std::cout << std::endl << "words startingLinks:" << std::endl;
    json grid_startingLinks_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json json_row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            json json_link = json::array();
            int num_links = startingWords[i][j].get_size(); // Ottieni il numero di collegamenti

            std::cout << "[" << i << "][" << j << "]: ";
            for (int k = 0; k < num_links; ++k) {
                int value = startingWords[i][j].get_value(k);
                std::cout << value << " ";
                json_link.push_back(value);
            }
            json_row.push_back(json_link);
            std::cout << std::endl;
        }
        grid_startingLinks_json.push_back(json_row);
    }


    // creo il contenuto JSON finale
    json data;
    data["grid"] = grid_json;
    data["words"] = words_json;
    data["grid_passingLinks"] = grid_passingLinks_json;
    data["grid_startingLinks"] = grid_startingLinks_json;

    // Salvare il JSON in un file
    String json_name = "quadrati_" + std::to_string(SERIAL_NUMBER) + ".json";

    std::ofstream file(json_name); //da generare un nome basato sul giorno (TODO)
    if (file.is_open()) {
        file << std::setw(2) << data << std::endl;  // Usa std::setw(2) per una formattazione leggibile
        file.close();
        std::cout << "File JSON generato correttamente!" << std::endl;
    } else {
        std::cerr << "Errore durante l'apertura del file!" << std::endl;
    }

    return 0;
}


/***********************************************************************************
// FUNZIONI
***********************************************************************************/
