#include "dizionario.h"

#include <iostream>
#include <fstream>

Dizionario::Dizionario() : radice(std::make_unique<Lettera>()) {}

void Dizionario::inserisciParola(const std::string& parola, Etichette etichette) {
    std::string parolaPulita = rimuoviAccenti(parola);
    Lettera* corrente = radice.get();
    for (char c : parolaPulita) {
        corrente = corrente->aggiungiFiglio(c);
    }
    corrente->fineParola = true;
    corrente->etichette.aggiungiEtichetta(etichette); // Aggiunge le etichette specificate
}

bool Dizionario::rimuoviParola(const std::string& parola) {
    std::string parolaPulita = rimuoviAccenti(parola);
    return rimuoviParolaRicorsivo(radice.get(), parolaPulita, 0);
}

bool Dizionario::riscriviEtichettaParola(const std::string& parola, Etichette etichette) {
    if (rimuoviParola(parola)) {
        inserisciParola(parola, etichette);
        return true;
    }
    return false;
}

std::optional<Etichette> Dizionario::cercaParola(const std::string& parola, Etichette etichette, bool OR_tra_etichette) const {
    std::string parolaPulita = rimuoviAccenti(parola);
    Lettera* corrente = radice.get();
    for (char c : parolaPulita) {
        corrente = corrente->getFiglio(c);
        if (!corrente) {
            return std::nullopt; // Lettera non trovata
        }
    }

    bool etichettaValida = OR_tra_etichette
                               ? corrente->etichette.haUnaEtichetta(etichette)  // Controlla se almeno un bit è attivo
                               : corrente->etichette.haTutteEtichette(etichette);  // Controlla se tutte le etichette coincidono esattamente

    return (corrente->fineParola && etichettaValida) ? std::optional<Etichette>(corrente->etichette) : std::nullopt;
}

int Dizionario::contaParole() const {
    return contaParoleRicorsivo(radice.get());
}

int Dizionario::contaParoleConEtichetta(Etichette etichetta, bool OR_tra_etichette) const {
    return contaParoleConEtichettaRicorsivo(radice.get(), etichetta, OR_tra_etichette);
}

bool Dizionario::salvaInFileCompatto(const std::string& percorsoFile) const {
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

bool Dizionario::caricaDaFileCompatto(const std::string& percorsoFile) {
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

bool Dizionario::rimuoviParolaRicorsivo(Lettera* nodo, const std::string& parola, int indice) {
    if (indice == parola.size()) {
        if (!nodo->fineParola) {
            return false;
        }
        nodo->fineParola = false;
        return nodo->figli.empty();
    }

    char c = parola[indice];
    Lettera* figlio = nodo->getFiglio(c);
    if (!figlio) {
        return false;
    }

    bool eliminaFiglio = rimuoviParolaRicorsivo(figlio, parola, indice + 1);

    if (eliminaFiglio) {
        nodo->rimuoviFiglio(c);
    }

    return nodo->figli.empty() && !nodo->fineParola;
}

int Dizionario::contaParoleRicorsivo(const Lettera* nodo) const {
    int count = nodo->fineParola ? 1 : 0;
    for (const auto& pair : nodo->figli) {
        const std::unique_ptr<Lettera>& figlio = pair.second;
        count += contaParoleRicorsivo(figlio.get());
    }
    return count;
}

int Dizionario::contaParoleConEtichettaRicorsivo(const Lettera* nodo, Etichette etichette, bool OR_tra_etichette) const {
    bool etichettaValida = OR_tra_etichette
                               ? nodo->etichette.haUnaEtichetta(etichette)
                               : nodo->etichette.haTutteEtichette(etichette);
    int count = (nodo->fineParola && etichettaValida) ? 1 : 0;
    for (const auto& pair : nodo->figli) {
        const std::unique_ptr<Lettera>& figlio = pair.second;
        count += contaParoleConEtichettaRicorsivo(figlio.get(), etichette, OR_tra_etichette);
    }
    return count;
}

// Mappa dei caratteri accentati e le corrispondenti vocali senza accento (con wchar_t)
std::string Dizionario::rimuoviAccenti(const std::string& input) const {
    // Converti std::string in std::wstring
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wide_input = converter.from_bytes(input);

    // Definisci la mappa dei caratteri accentati
    static const std::unordered_map<wchar_t, wchar_t> accenti{
        //{'à', 'a'}, {'è', 'e'}, {'é', 'e'}, {'ì', 'i'}, {'ò', 'o'}, {'ù', 'u'},
        //{'À', 'A'}, {'È', 'E'}, {'É', 'E'}, {'Ì', 'I'}, {'Ò', 'O'}, {'Ù', 'U'}
        {L'\u00E0', L'a'}, {L'\u00E8', L'e'}, {L'\u00E9', L'e'}, {L'\u00EC', L'i'}, {L'\u00F2', L'o'}, {L'\u00F9', L'u'},
        {L'\u00C0', L'A'}, {L'\u00C8', L'E'}, {L'\u00C9', L'E'}, {L'\u00CC', L'I'}, {L'\u00D2', L'O'}, {L'\u00D9', L'U'}
    };

    std::wstring wide_output;
    wide_output.reserve(wide_input.size()); // Prealloca la memoria

    for (wchar_t c : wide_input) {
        if (accenti.count(c)) {
            wide_output += accenti.at(c); // Sostituisci se è accentato
        } else if (c >= 0 && c < 128) {
            wide_output += c; // Aggiungi i caratteri ASCII non accentati
        }
    }

    // Converti std::wstring in std::string
    std::string output = converter.to_bytes(wide_output);
    return output;
}
