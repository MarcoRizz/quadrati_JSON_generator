#ifndef GESTISCI_DIZIONARI_H
#define GESTISCI_DIZIONARI_H

#include <iostream>

enum class ParolaStatus {
    Accettata,
    Bonus,
    Rifiutata,
    Sconosciuta
};

bool contieneParola(const std::string& filePath, const std::string& parola);
bool rimuoviParola(const std::string& filePath, const std::string& parola);
bool aggiungiParola(const std::string& filePath, const std::string& parola);

#endif