#ifndef GENERATE_JSON_H
#define GENERATE_JSON_H

using String = std::string;

const int DIM1 = 4;
const int DIM2 = 4;
const int MAX_LENGTH = 16; // Lunghezza massima delle parole

extern int search_word(const std::string& word, const std::string& dir);
extern void add_word(String*& array, int& size, const String& word);

extern char grid[DIM1][DIM2];
extern int words_size;
extern String* words;

#endif