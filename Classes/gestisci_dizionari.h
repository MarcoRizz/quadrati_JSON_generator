#ifndef GESTISCI_DIZIONARI_H
#define GESTISCI_DIZIONARI_H

#include <iostream>

enum class ParolaStatus {
    Accettata,
    Bonus,
    Rifiutata,
    Sconosciuta
};

ParolaStatus ask_the_boss(const std::string& parola);
bool contieneParola(std::ifstream& file, const std::string& parola);
bool rimuoviParola(std::ifstream& file, const std::string& filePath, const std::string& parola);
bool aggiungiParola(std::ifstream& file, const std::string& filePath, const std::string& parola);

#endif