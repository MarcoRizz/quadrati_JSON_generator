#include <iostream>
//#include <chrono> // Libreria per il timing
#include "generate_JSON.h"
#include "libs/dictionary_dir.h"
#include "dictionary_ref.h"

long n_paths = 0;

bool visited[DIM1][DIM2] = {false}; // Array di visitati
std::pair<int, int> path[DIM1 * DIM2]; // Array per memorizzare il percorso

// Direzioni: U, UR, R, DR, D, DL, L, UL
const std::pair<int, int> directions[8] = {
    {0, -1}, {1, -1}, {1, 0}, {1, 1},
    {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

bool isValid(int x, int y) {
    return x >= 0 && x < DIM1 && y >= 0 && y < DIM2 && !visited[x][y];
}

void printPath(int pathLength) {
    /*for (int i = 0; i < pathLength; ++i) {
        std::cout << "(" << path[i].first << "," << path[i].second << ") ";
    }*/
    n_paths++;
    if (n_paths % 100000 == 0) {
        printf("%d\n", n_paths);
    }
    //std::cout << std::endl;
    /*String parola;
    for (int i = 0; i < pathLength; ++i) {
        parola += grid[path[i].first][path[i].second];
    }
    if (search_word(parola, DICTIONARY_PATH)) {
        add_word(words, words_size, parola);
    }*/
}

void findPaths(int x, int y, int step, int path_size) {
    path[step] = {x, y};
    visited[x][y] = true;

    // Se abbiamo raggiunto il numero di passi massimo, stampiamo il percorso
    if (step + 1 == path_size) {
        printPath(step + 1);
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
/*
int main() {
    // Inizio misurazione tempo
    auto start = std::chrono::high_resolution_clock::now();

    // Ciclo per path_size da 4 a 16
    for (int path_size = 4; path_size <= 16; ++path_size) {
        std::cout << "Percorsi con " << path_size << " passi:" << std::endl;
        for (int i = 0; i < DIM1; ++i) {
            for (int j = 0; j < DIM2; ++j) {
                findPaths(i, j, 0, path_size);
            }
        }
        std::cout << std::endl;
    }
    
    // Fine misurazione tempo
    auto end = std::chrono::high_resolution_clock::now();

    // Calcolo del tempo trascorso
    std::chrono::duration<double> elapsed = end - start;
    printf("%d\n", n_paths);
    std::cout << "Tempo totale di esecuzione: " << elapsed.count() << " secondi" << std::endl;

    return 0;
}
*/