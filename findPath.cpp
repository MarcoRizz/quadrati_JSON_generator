#include <iostream>
#include "findPath.h"
#include "generate_JSON.h"

#define DIRECTIONS_n 8

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
    if (dizionario.cercaParola(parola)) {
        words.add_word(parola);
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

void returnFinalPath(int pathLength, int wordIndex) {
    bool nuovoPath = false;
    for (int i = 0; i < pathLength; ++i) {
        nuovoPath |= passingWords[path[i].first][path[i].second].add_value(wordIndex);
        if (i == 0) {
            startingWords[path[i].first][path[i].second].add_value(wordIndex);
        }
    }
    if (nuovoPath) {
        std::cout << words.get_word(wordIndex) << " -> nuovo path: ";
        for (int i = 0; i < pathLength; ++i) {
            std::cout << "{" << path[i].first << ", " << path[i].second << "}, ";
        }
        std::cout << std::endl;
    }
}

//cerca tutti i possibili percorsi di una specifica parola
void findWordPaths(int x, int y, int step, String word, int wordIndex) {
    path[step] = {x, y};
    visited[x][y] = true;

    // Se abbiamo raggiunto il numero di passi massimo, stampiamo il percorso
    if (step + 1 == word.size()) {
        returnFinalPath(step + 1, wordIndex);
    } else {
        for (int i = 0; i < DIRECTIONS_n; ++i) {
            int newX = x + directions[i].first;
            int newY = y + directions[i].second;
            if (isValid(newX, newY) && grid[newX][newY] == word[step + 1]) {
                findWordPaths(newX, newY, step + 1, word, wordIndex);
            }
        }
    }

    // Backtracking
    visited[x][y] = false;
}