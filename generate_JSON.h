#ifndef GENERATE_JSON_H
#define GENERATE_JSON_H

#include "Classes/WordList.h"
#include "Classes/Dizionario.h"
#include "Classes/DynArray.h"

using String = std::string;

#define DIZIONARIO_TXT_ACCETTATE_PATH "tools\\Dizionari\\dizionario_parole_accettate.txt"
#define DIZIONARIO_TXT_BONUS_PATH "tools\\Dizionari\\dizionario_parole_bonus.txt"
#define DIZIONARIO_TXT_RIFIUTATE_PATH "tools\\Dizionari\\dizionario_parole_rifiutate.txt"

const int DIM1 = 4;
const int DIM2 = 4;
const int MAX_LENGTH = 16; // Lunghezza massima delle parole

extern int n_paths;

extern char grid[DIM1][DIM2];
extern WordList words;
extern WordList words_bonus;
extern Dizionario dizionario;
extern std::ifstream dizionario_txt_accettate;
extern std::ifstream dizionario_txt_bonus;
extern std::ifstream dizionario_txt_rifiutate;

extern std::vector<std::vector<DynArray>> passingWords;

#endif