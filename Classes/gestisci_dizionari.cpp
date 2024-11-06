#include <vector>
#include <string>
#include <fstream>
#include <algorithm>
#include <iostream>

bool contieneParola(const std::string& filePath, const std::string& parola) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Errore: impossibile aprire " << filePath << "\n";
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line == parola) {
            return true;
        }
    }
    return false;
}

bool rimuoviParola(const std::string& filePath, const std::string& parola) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "Errore: impossibile aprire " << filePath << "\n";
        return false;
    }

    std::vector<std::string> parole;
    std::string line;
    bool found = false;

    while (std::getline(file, line)) {
        if (line == parola) {
            found = true;
        } else {
            parole.push_back(line);
        }
    }
    file.close();

    if (!found) {
        std::cout << "La parola \"" << parola << "\" non è presente nel dizionario.\n";
        return false;
    }

    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "Errore: impossibile aprire " << filePath << " per la scrittura\n";
        return false;
    }

    for (const auto& p : parole) {
        outFile << p << "\n";
    }
    return true;
}

bool aggiungiParola(const std::string& filePath, const std::string& parola) {
    std::vector<std::string> parole;
    std::string line;
    bool alreadyExists = false;

    std::ifstream file(filePath);
    if (file.is_open()) {
        while (std::getline(file, line)) {
            if (line == parola) {
                alreadyExists = true;
                break;
            }
            parole.push_back(line);
        }
        file.close();
    }

    if (alreadyExists) {
        std::cout << "La parola \"" << parola << "\" è già presente nel dizionario.\n";
        return false;
    }

    parole.push_back(parola);
    std::sort(parole.begin(), parole.end());

    std::ofstream outFile(filePath);
    if (!outFile.is_open()) {
        std::cerr << "Errore: impossibile aprire " << filePath << " per la scrittura\n";
        return false;
    }

    for (const auto& p : parole) {
        outFile << p << "\n";
    }
    return true;
}
