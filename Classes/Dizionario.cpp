#include "Dizionario.h"

Dizionario::Dizionario() : radice(std::make_unique<Lettera>()) {}

void Dizionario::inserisciParola(const String& parola) {
    Lettera* corrente = radice.get();
    for (char c : parola) {
        corrente = corrente->aggiungiFiglio(c);
    }
    corrente->fineParola = true; // Contrassegna la fine della parola
}

bool Dizionario::cercaParola(const String& parola) const {
    Lettera* corrente = radice.get();
    for (char c : parola) {
        corrente = corrente->getFiglio(c);
        if (!corrente) {
            return false; // Lettera non trovata
        }
    }
    return corrente->fineParola; // Verifica se è la fine di una parola
}

int Dizionario::contaParole() const {
    return contaParoleRicorsivo(radice.get());
}

bool Dizionario::salvaInFileCompatto(const String& percorsoFile) const {
    json j = radice->to_json_compatto();
    std::ofstream file(percorsoFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Errore nell'aprire il file per la scrittura: " << percorsoFile << std::endl;
        return false;
    }
    file << j.dump(); // Dump senza indentazione per risparmiare spazio
    file.close();
    return true;
}

bool Dizionario::caricaDaFileCompatto(const String& percorsoFile) {
    std::ifstream file(percorsoFile, std::ios::binary);
    if (!file.is_open()) {
        std::cerr << "Errore nell'aprire il file per la lettura: " << percorsoFile << std::endl;
        return false;
    }
    json j;
    file >> j;
    file.close();

    radice = Lettera::from_json_compatto(j);
    return true;
}

int Dizionario::contaParoleRicorsivo(const Lettera* nodo) const {
    int count = nodo->fineParola ? 1 : 0;
    for (const auto& pair : nodo->figli) {
        const std::unique_ptr<Lettera>& figlio = pair.second;
        count += contaParoleRicorsivo(figlio.get());
    }
    return count;
}