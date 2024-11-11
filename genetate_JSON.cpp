#include <iostream>
#include <fstream>
#include <cstdlib>  // Per rand() e srand()
#include <ctime>    // Per time()
#include <chrono>
#include <string>
#include <ctime>
#include <cmath>  // Per std::difftime e std::abs
#include "generate_JSON.h"
#include "libs/json.hpp"
#include "findPath.h"

//#define PATH_MAX_STEPS 15
#define MAX_LOOPS 10

using json = nlohmann::json;

char grid[DIM1][DIM2];
WordList words;
WordList words_bonus;
Dizionario dizionario;
const std::string dictionary_path_json = (std::string)"tools\\Dizionari\\dizionario.json";
std::vector<std::vector<DynArray>> passingWords(DIM1, std::vector<DynArray>(DIM2, DynArray(words.get_size())));

int n_words_old = 0;
int n_paths_old = 0;
int n_paths = 0;

int calcolaDifferenzaGiorni(const std::tm& giorno1, const std::tm& giorno2);

int main() {
    // Giorno di lancio: 1 gennaio 2024
    std::tm giornoDiLancioTm = {};
    giornoDiLancioTm.tm_year = 2024 - 1900; // Anno (2024: 2024 - 1900)
    giornoDiLancioTm.tm_mon = 8;            // Mese (gennaio: 0 in 0-based)
    giornoDiLancioTm.tm_mday = 26;           // Giorno del mese

    // Giorno X: 20 maggio 2024
    std::tm giornoX = {};
    giornoX.tm_year = 2024 - 1900; // Anno (2024: 2024 - 1900)
    giornoX.tm_mon = 10;            // Mese (maggio: 4 in 0-based)
    //giornoX.tm_mday = 3;          // Giorno del mese

    // Inizializza il seme del generatore di numeri casuali
    auto timer_overall_start = std::chrono::high_resolution_clock::now();
    std::srand(std::time(0));

    //apro il dizionario
    if (!dizionario.caricaDaFileCompatto(dictionary_path_json)) {
        std::cout << "errore nell'apertura del file al percorso: " << dictionary_path_json << std::endl;
        return -1;
    }

    auto timer_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = timer_end - timer_overall_start;
    std::cout << "Dizionario caricato - elapsed time: " << duration.count() << " ms" << std::endl;
    
    for (giornoX.tm_mday = 20; giornoX.tm_mday < 21; ++giornoX.tm_mday) {
        n_words_old = 0;
        n_paths_old = 0;
        n_paths = 0;

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
                    std::cout << std::string(1, grid[i][j] + 'A' - 'a').c_str() << " ";
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
            for (
                    #ifdef PATH_MAX_STEPS
                        int path_size = 4; path_size <= PATH_MAX_STEPS; ++path_size
                    #else
                        int path_size = 4; path_size <= DIM1 * DIM2; ++path_size
                    #endif
                ) {
                for (int i = 0; i < DIM1; ++i) {
                    for (int j = 0; j < DIM2; ++j) {
                        findPaths(i, j, 0, path_size);  //qui dentro riempio words
                    }
                }
            }

            timer_end = std::chrono::high_resolution_clock::now();
            duration = timer_end - timer_overall_start;
            std::cout << "Array WORDS: "<< words.get_size() - n_words_old << " nuove parole - elapsed time: " << duration.count() << " ms" << std::endl;
            n_words_old = words.get_size();
            
            /***********************************************************************************
            // CREAZIONE ARRAY-3D GRID_LINKS
            ***********************************************************************************/
            //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (e quali possono iniziare)

            for (int word_i = 0; word_i < words.get_size(); ++word_i) {
                std::string running_word = words.get_word_by_insertion(word_i);
                std::pair<int, int> running_start = words.get_startingTile_by_insertion(word_i);
                for (int i = 0; i < DIM1; ++i) {
                    for (int j = 0; j < DIM2; ++j) {
                        if(grid[i][j] == running_word[0]) {
                            findWordPaths(i, j, 0, running_word, word_i, running_start);  //qui dentro riempio passingWords
                        }
                    }
                }
                words.add_startingTile_by_insertion(running_start, word_i); // e con questo completo words.startingTile
            }

            timer_end = std::chrono::high_resolution_clock::now();
            duration = timer_end - timer_overall_start;
            std::cout << "Array PASSINGWORDS: "<< n_paths - n_paths_old << " nuovi percorsi - elapsed time: " << duration.count() << " ms" << std::endl;
            n_paths_old = n_paths;
            //se una lettera rimane priva di link, dovrò sostituirla e ripetere il calcolo
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
            std::cerr << "Numero massimo di iterazioni raggiunto, griglia non trovata" << std::endl;
            
            //riazzero le variabili ad ogni iterazione
            words.clear();
            passingWords.clear();
            passingWords = std::vector<std::vector<DynArray>>(DIM1, std::vector<DynArray>(DIM2, DynArray(words.get_size())));

            --giornoX.tm_mday;  //torno indietro per ripetere il giorno

            continue;
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
                row.push_back(std::string(1, grid[i][j]));  // converto il char in stringa per il JSON
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
        // Converto le parole in JSON
        json words_json = json::array();
        for (int i = 0; i < words.get_size(); ++i) {
            std::string word_i = words.get_word_by_alphabetical(i);
            std::cout << "#" << i << ":" << word_i << std::endl;
            std::transform(word_i.begin(), word_i.end(), word_i.begin(), ::toupper); // Converte ogni carattere in maiuscolo
            words_json.push_back(word_i);
        }

        // Converto passingWords in JSON
        std::cout << std::endl << "words passingLinks:" << std::endl;
        json passingLinks_json = json::array();
        for (int i = 0; i < DIM1; ++i) {
            json json_row = json::array();
            for (int j = 0; j < DIM2; ++j) {
                json json_link = json::array();
                int num_links = passingWords[i][j].get_size(); // Ottieni il numero di collegamenti

                std::cout << "{" << i << ", " << j << "}: ";
                DynArray alphabetical_index;
                for (int k = 0; k < num_links; ++k) {
                    alphabetical_index.add_value(words.get_alphabetical_index(passingWords[i][j].get_value(k)));
                }
                for (int k = 0; k < num_links; ++k) {
                    int value = alphabetical_index.get_value(k);
                    std::cout << value << " ";
                    json_link.push_back(value);
                }
                json_row.push_back(json_link);
                std::cout << std::endl;
            }
            passingLinks_json.push_back(json_row);
        }

        // Converti startingLinks in JSON
        std::cout << std::endl << "words startingLinks:" << std::endl;
        json startingLinks_json = json::array();
        for (int i = 0; i < words.get_size(); ++i) {
            json json_pair = json::array();
            std::pair<int, int> startingTile = words.get_startingTile_by_alphabetical(i);
            std::cout << "#" << i << " -> {" << startingTile.first << ", " << startingTile.second << "}" << std::endl;
            json_pair.push_back(startingTile.first);
            json_pair.push_back(startingTile.second);
            startingLinks_json.push_back(json_pair);
        }

        // Converto words_bonus in JSON
        json words_bonus_json = json::array();
        for (int i = 0; i < words_bonus.get_size(); ++i) {
            std::string word_bonus_i = words_bonus.get_word_by_alphabetical(i);

            bool in_grid = false;
            for (int i = 0; i < DIM1; ++i) {
                for (int j = 0; j < DIM2; ++j) {
                    in_grid = is_still_in_grid(i, j, 0, word_bonus_i);
                    if (in_grid) {
                        break;
                    }
                }
                if (in_grid) {
                    break;
                }
            }
            if (in_grid) {
                std::cout << "#" << i << ":" << word_bonus_i << std::endl;
                std::transform(word_bonus_i.begin(), word_bonus_i.end(), word_bonus_i.begin(), ::toupper); // Converte ogni carattere in maiuscolo
                words_bonus_json.push_back(word_bonus_i);
            }
        }

        // creo il contenuto JSON finale
        int todaysNum = calcolaDifferenzaGiorni(giornoDiLancioTm, giornoX) + 1;
        json data;
        data["todaysNum"] = todaysNum;
        data["grid"] = grid_json;
        data["words"] = words_json;
        data["passingLinks"] = passingLinks_json;
        data["startingLinks"] = startingLinks_json;
        data["bonus"] = words_bonus_json;

        // Salvare il JSON in un file
        std::string json_name = "quadrati#" + std::to_string(todaysNum) + ".json";

        std::ofstream file(json_name);
        if (file.is_open()) {
            file << std::setw(2) << data << std::endl;  // Usa std::setw(2) per una formattazione leggibile
            file.close();
            std::cout << "File JSON #" + std::to_string(todaysNum) + "generato correttamente --> giorno " + std::asctime(&giornoX) << std::endl;
        } else {
            std::cerr << "Errore durante l'apertura del file!" << std::endl;
        }

        //riazzero le variabili ad ogni iterazione
        words.clear();
        passingWords.clear();
        passingWords = std::vector<std::vector<DynArray>>(DIM1, std::vector<DynArray>(DIM2, DynArray(words.get_size())));
    }

    // chiedo se salvare le modifiche al dizionario
    char input;
    while (true) {
        std::cout << "Salvare le modifiche del dizionario? S/N" << std::endl;
        std::cin >> input;

        switch (input) {
            case 'S':
                if (!dizionario.salvaInFileCompatto(dictionary_path_json)) {
                    std::cout << "errore nel salvataggio del dizionario al percorso: " << dictionary_path_json << std::endl;
                    return -1;
                }
                return 0;

            case 'N':
                return 0;

            default:
                std::cout << "Comando non valido. Riprova.\n";
                break;
        }
    }
}


/***********************************************************************************
// FUNZIONI
***********************************************************************************/

int calcolaDifferenzaGiorni(const std::tm& giorno1, const std::tm& giorno2) {
    // Converti le strutture tm in time_t (secondi dal 1 gennaio 1970)
    std::time_t time1 = std::mktime(const_cast<std::tm*>(&giorno1));
    std::time_t time2 = std::mktime(const_cast<std::tm*>(&giorno2));

    // Calcola la differenza in secondi e dividila per i secondi in un giorno
    double diffInSeconds = std::difftime(time2, time1);
    int diffInDays = std::abs(diffInSeconds / (60 * 60 * 24));  // Converti i secondi in giorni

    return diffInDays;
}