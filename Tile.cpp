#include "Tile.h"
#include <cstring> // Per memcpy


void Tile::resize1(int new_capacity) {
    int* new_passingWord = new int[new_capacity];
    std::memcpy(new_passingWord, passingWord, size1 * sizeof(int));
    delete[] passingWord;
    passingWord = new_passingWord;
    capacity1 = new_capacity;
}

void Tile::resize2(int new_capacity) {
    int* new_startingWord = new int[new_capacity];
    std::memcpy(new_startingWord, startingWord, size2 * sizeof(int));
    delete[] startingWord;
    startingWord = new_startingWord;
    capacity2 = new_capacity;
}

Tile::Tile(int initial_capacity)
    : size1(0), capacity1(initial_capacity), size2(0), capacity2(initial_capacity) {
    passingWord = new int[capacity1];
    startingWord = new int[capacity2];
}

Tile::~Tile() {
    delete[] passingWord;
    delete[] startingWord;
}

void Tile::giveSize(int size) {
    resize1(size);
    resize2(size);
}

void Tile::add_passingWord(int value) {
    // Se l'array è pieno, rialloca con una capacità maggiore
    if (size1 == capacity1) {
        resize1(capacity1 + 1);
    }
    passingWord[size1] = value;
    size1++;
}

void Tile::add_startingWord(int value) {
    // Se l'array è pieno, rialloca con una capacità maggiore
    if (size2 == capacity2) {
        resize2(capacity2 + 1);
    }
    startingWord[size2] = value;
    size2++;
}