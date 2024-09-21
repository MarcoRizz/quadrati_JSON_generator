#include <iostream>
#include "findPath.h"
#include "generate_JSON.h"

bool visited[DIM1][DIM2] = {false}; // Array di visitati
std::pair<int, int> path[DIM1 * DIM2]; // Array per memorizzare il percorso

// Direzioni: U, UR, R, DR, D, DL, L, UL
const std::pair<int, int> directions[8] = {
    {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

bool isValid(int x, int y) {
    return x >= 0 && x < DIM1 && y >= 0 && y < DIM2 && !visited[x][y];
}

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

void returnFinalPath(int pathLength) {
    //TODO
}

//cerca tutti i possibili percorsi di una specifica parola
void findWordPaths(int x, int y, int step, String word) {
    path[step] = {x, y};
    visited[x][y] = true;

    // Se abbiamo raggiunto il numero di passi massimo, stampiamo il percorso
    if (step + 1 == word.size()) {
        returnFinalPath(step + 1);
    } else {
        for (int i = 0; i < 8; ++i) {
            int newX = x + directions[i].first;
            int newY = y + directions[i].second;
            if (isValid(newX, newY) && grid[path[i].first][path[i].second] == word[step]) {
                findWordPaths(newX, newY, step + 1, word);
            }
        }
    }

    // Backtracking
    visited[x][y] = false;
}