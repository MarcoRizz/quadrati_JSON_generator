#ifndef GENERATE_JSON_H
#define GENERATE_JSON_H

#include "Classes/WordList.h"
#include "Classes/Dizionario.h"
#include "Classes/DynArray.h"

const int DIM1 = 4;
const int DIM2 = 4;
const int MAX_LENGTH = 16; // Lunghezza massima delle parole

extern int n_paths;

extern char grid[DIM1][DIM2];
extern WordList words;
extern WordList words_bonus;
extern Dizionario dizionario;

extern std::vector<std::vector<DynArray>> passingWords;

#endif