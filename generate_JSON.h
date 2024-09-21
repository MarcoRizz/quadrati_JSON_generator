#ifndef GENERATE_JSON_H
#define GENERATE_JSON_H

#include "WordList.h"
#include "Dizionario.h"
#include "DinArray.h"

using String = std::string;

const int DIM1 = 4;
const int DIM2 = 4;
const int MAX_LENGTH = 16; // Lunghezza massima delle parole

extern char grid[DIM1][DIM2];
extern WordList words;
extern Dizionario dizionario;

extern std::vector<std::vector<DinArray>> passingWords;
extern std::vector<std::vector<DinArray>> startingWords;

#endif