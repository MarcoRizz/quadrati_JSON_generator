#include <iostream>
#include "findPath.h"
#include "generate_JSON.h"
#include "Classes/gestisci_dizionari.h"

#define DIRECTIONS_n 8

ParolaStatus consulta_dizionari(const std::string& parola);

bool visited[DIM1][DIM2] = {false}; // Array di visitati
std::pair<int, int> path[DIM1 * DIM2]; // Array per memorizzare il percorso

// Direzioni: U, UR, R, DR, D, DL, L, UL
const std::pair<int, int> directions[DIRECTIONS_n] = {
    {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

bool isValid(int x, int y) {
    return x >= 0 && x < DIM1 && y >= 0 && y < DIM2 && !visited[x][y];
}

//--------------------------------------------------------------------------------
//ELABORO TUTTI I POSSIBILI PERCORSI NELLA GRIGLIA

void returnFinalWord(int pathLength) {
    String parola;
    for (int i = 0; i < pathLength; ++i) {
        parola += grid[path[i].first][path[i].second];
    }
    if (dizionario.cercaParola(parola)) { //se trovo la parola nel json
        //consulto i dizionari
        ParolaStatus status = consulta_dizionari(parola);
        switch (status) {
            case ParolaStatus::Accettata:
                words.add_word(parola);
                break;
            case ParolaStatus::Bonus:
                words_bonus.add_word(parola);
                break;
            case ParolaStatus::Rifiutata:
                dizionario.rimuoviParola(parola);
                break;
            default:
                std::cout << "ERRORE CODICE\n";
                break;
        }
    }
}

//cerca tutti i possibili percorsi nella griglia
void findPaths(int x, int y, int step, int path_size) {
    path[step] = {x, y};
    visited[x][y] = true;

    // Se abbiamo raggiunto il numero di passi massimo, stampiamo il percorso
    if (step + 1 == path_size) {
        returnFinalWord(step + 1);
    } else {
        for (int i = 0; i < 8; ++i) {
            int newX = x + directions[i].first;
            int newY = y + directions[i].second;
            if (isValid(newX, newY)) {
                findPaths(newX, newY, step + 1, path_size);
            }
        }
    }

    // Backtracking
    visited[x][y] = false;
}

//--------------------------------------------------------------------------------
//IDENTIFICO I PERCORSI DI UNA PAROLA

void returnFinalPath(int pathLength, int wordIndex, std::pair<int, int>& startingtile) {
    bool nuovoPath = false;
    for (int i = 0; i < pathLength; ++i) { 
        nuovoPath |= passingWords[path[i].first][path[i].second].add_value(wordIndex);
        if (i == 0 && startingtile == std::make_pair(-1, -1)) {
            startingtile = path[i];
        }
    }
    if (nuovoPath) {
        std::cout << words.get_word_by_insertion(wordIndex) << " -> nuovo path: ";
        for (int i = 0; i < pathLength; ++i) {
            std::cout << "{" << path[i].first << ", " << path[i].second << "}, ";
        }
        std::cout << std::endl;
        n_paths++;
    }
}

//cerca tutti i possibili percorsi di una specifica parola
void findWordPaths(int x, int y, int step, String word, int wordIndex, std::pair<int, int>& startingtile) {
    path[step] = {x, y};
    visited[x][y] = true;

    // Se abbiamo raggiunto il numero di passi massimo, stampiamo il percorso
    if (step + 1 == word.size()) {
        returnFinalPath(step + 1, wordIndex, startingtile);
    } else {
        for (int i = 0; i < DIRECTIONS_n; ++i) {
            int newX = x + directions[i].first;
            int newY = y + directions[i].second;
            if (isValid(newX, newY) && grid[newX][newY] == word[step + 1]) {
                findWordPaths(newX, newY, step + 1, word, wordIndex, startingtile);
            }
        }
    }

    // Backtracking
    visited[x][y] = false;
}


//--------------------------------------------------------------------------------
//VALUTO SE LA PAROLA È ANCORA NELLA GRIGLIA

// Cerca il primo percorso possibile per una specifica parola
bool is_still_in_grid(int x, int y, int step, const String& word) {
    // Segna la posizione attuale
    path[step] = {x, y};
    
    // Se abbiamo raggiunto la lunghezza della parola, stampiamo il percorso
    if (step + 1 == word.size()) {
        return true; // Percorso trovato
    }

    // Esplora le direzioni
    for (int i = 0; i < DIRECTIONS_n; ++i) {
        int newX = x + directions[i].first;
        int newY = y + directions[i].second;

        // Controlla se la nuova posizione è valida, se corrisponde alla lettera successiva della parola
        // e se non corrisponde a uno step precedente di path
        bool isStepPreviouslyVisited = false;
        for (int j = 0; j <= step; ++j) {
            if (path[j] == std::make_pair(newX, newY)) {
                isStepPreviouslyVisited = true;
                break;
            }
        }

        if (isValid(newX, newY) && grid[newX][newY] == word[step + 1] && !isStepPreviouslyVisited) {
            if (is_still_in_grid(newX, newY, step + 1, word)) {
                return true; // Se troviamo un percorso, restituiamo true
            }
        }
    }

    // Non troviamo alcun percorso
    return false;
}


ParolaStatus consulta_dizionari(const std::string& parola) {
    
    if (contieneParola(dizionario_txt_accettate, parola)) {
        return ParolaStatus::Accettata;
    }
    if (contieneParola(dizionario_txt_bonus, parola))
    {
        return ParolaStatus::Bonus;
    }
    if (contieneParola(dizionario_txt_rifiutate, parola))
    {
        return ParolaStatus::Rifiutata;
    }
    
    ParolaStatus status = ask_the_boss(parola);
    switch (status) {
        case ParolaStatus::Accettata:
            aggiungiParola(dizionario_txt_accettate, DIZIONARIO_TXT_ACCETTATE_PATH, parola);
            std::cout << "Parola accettata!\n";
            break;
        case ParolaStatus::Bonus:
            aggiungiParola(dizionario_txt_bonus, DIZIONARIO_TXT_BONUS_PATH, parola);
            std::cout << "Parola bonus!\n";
            break;
        case ParolaStatus::Rifiutata:
            aggiungiParola(dizionario_txt_rifiutate, DIZIONARIO_TXT_RIFIUTATE_PATH, parola);
            std::cout << "Parola rifiutata!\n";
            break;
        default:
            std::cout << "Parola sconosciuta.\n";
            break;
    }
    return status;
}