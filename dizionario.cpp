#include "dizionario.h"

#include <iostream>
#include <fstream>
#include <qdebug.h>
#include <qlogging.h>

Dizionario::Dizionario() : radice(std::make_unique<Lettera>()) {}

bool Dizionario::isLoaded() {
    return !radice->figli.empty();
}

// Se la parola era già presente, aggiunge le etichette a quelle già esistenti
void Dizionario::inserisciParola(const std::string& parola, Etichette etichette, bool sovrascriviEtichette) {
    std::string parolaPulita = rimuoviAccenti(parola);
    Lettera* corrente = radice.get();
    for (char c : parolaPulita) {
        corrente = corrente->aggiungiFiglio(c);
    }
    corrente->fineParola = true;
    if (sovrascriviEtichette) {
        corrente->etichette = Etichette::Nessuna;
    }
    corrente->etichette.aggiungiEtichetta(etichette); // Aggiunge le etichette specificate
}

bool Dizionario::rimuoviParola(const std::string& parola) {
    std::string parolaPulita = rimuoviAccenti(parola);
    return rimuoviParolaRicorsivo(radice.get(), parolaPulita, 0);
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

    if (!corrente->fineParola)
        return std::nullopt;


    bool etichettaValida = OR_tra_etichette
                               ? corrente->etichette.haUnaEtichetta(etichette)  // Controlla se almeno un bit è attivo
                               : corrente->etichette.haTutteEtichette(etichette);  // Controlla se tutte le etichette coincidono esattamente

    return etichettaValida ? std::optional<Etichette>(corrente->etichette) : std::nullopt;
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
        nodo->etichette = Etichette::Nessuna;
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

std::string Dizionario::cercaParolaSuccessiva(const std::string& parola) {

    std::string parolaPulita = rimuoviAccenti(parola);
    Lettera* corrente = radice.get();
    std::vector<std::pair<Lettera*, char>> percorso;

    for (char c : parolaPulita) {
        percorso.emplace_back(corrente, c);
        corrente = corrente->getFiglio(c);
        if (!corrente) {
            //throw std::runtime_error("Parola non trovata nel dizionario.");
            return "";
        }
    }

    // Prima controlla se ci sono figli successivi dell'ultimo nodo corrente
    std::string parolaCandidata = parolaPulita;
    Lettera* figlio;
    for (char c = 'a'; c <= 'z'; c++) {
        figlio = corrente->getFiglio(c);
        if (figlio) {
            parolaCandidata.push_back(c);
            if (completaParolaDaNodo(figlio, parolaCandidata, 'a'))
                return parolaCandidata;
            else
                parolaCandidata.pop_back();
        }

    }

    for (int i = static_cast<int>(percorso.size()) - 1; i >= 0; --i) {

        parolaCandidata.pop_back();

        if (completaParolaDaNodo(percorso[i].first, parolaCandidata, percorso[i].second + 1))
            return parolaCandidata;
    }

    return "";
}

bool Dizionario::completaParolaDaNodo(const Lettera* nodo, std::string& base, const char initialChar) const {
    if (nodo->fineParola) {
        return true;
    }

    for (char c = initialChar; c <= 'z'; c++) {
        Lettera* discendente = nodo->getFiglio(c);

        if (!discendente)
            continue;

        base.push_back(c);
        if (completaParolaDaNodo(discendente, base, 'a')) {
            return true;
        }
        base.pop_back();

    }

    return false;
}


std::string Dizionario::cercaParolaPrecedente(const std::string& parola) {

    std::string parolaPulita = rimuoviAccenti(parola);
    Lettera* corrente = radice.get();
    std::vector<std::pair<Lettera*, char>> percorso;

    for (char c : parolaPulita) {
        percorso.emplace_back(corrente, c);
        corrente = corrente->getFiglio(c);
        if (!corrente) {
            throw std::runtime_error("Parola non trovata nel dizionario.");
        }
    }

    std::string parolaCandidata = parolaPulita;
    Lettera* figlio;
    for (char c = 'z'; c >= 'a'; --c) {
        figlio = corrente->getFiglio(c);
        if (figlio) {
            parolaCandidata.push_back(c);
            if (completaParolaDaNodoPrecedente(figlio, parolaCandidata, 'z'))
                return parolaCandidata;
            else
                parolaCandidata.pop_back();
        }
    }

    for (int i = static_cast<int>(percorso.size()) - 1; i >= 0; --i) {
        parolaCandidata.pop_back();

        for (char c = static_cast<char>(percorso[i].second - 1); c >= 'a'; --c) {
            figlio = percorso[i].first->getFiglio(c);
            if (figlio) {
                parolaCandidata.push_back(c);
                if (completaParolaDaNodoPrecedente(figlio, parolaCandidata, 'z')) {
                    return parolaCandidata;
                } else {
                    parolaCandidata.pop_back();
                }
            }
            if (c == 'a') break;  // evitare overflow quando c diventa '\xFF'
        }
    }

    return "";
}


bool Dizionario::completaParolaDaNodoPrecedente(const Lettera* nodo, std::string& base, const char initialChar) const {
    if (nodo->fineParola) {
        return true;
    }

    for (char c = initialChar; c >= 'a'; --c) {
        Lettera* discendente = nodo->getFiglio(c);
        if (!discendente)
            continue;

        base.push_back(c);
        if (completaParolaDaNodoPrecedente(discendente, base, 'z')) {
            return true;
        }
        base.pop_back();

        if (c == 'a') break;  // evitare overflow
    }

    return false;
}

// --------------------------------------------------
// Ricerca parole in range
// --------------------------------------------------
std::vector<parola> Dizionario::cercaParoleInRange(const std::string& parola, const int up, const int down) {

    std::vector<struct parola> precedenti;
    std::vector<struct parola> successivi;
    struct parola centrale;

    // 1) Trova il nodo della parola
    std::string parolaPulita = rimuoviAccenti(parola);
    Lettera* corrente = radice.get();
    std::vector<std::pair<Lettera*, char>> percorso;
    for (char c : parolaPulita) {
        percorso.emplace_back(corrente, c);
        corrente = corrente->getFiglio(c);
        if (!corrente)
            return {};//throw std::runtime_error("Parola non trovata nel dizionario.");
    }

    // 2) Parola centrale
    if (corrente->fineParola) {
        centrale.voce = parolaPulita;
        centrale.etichetta = corrente->etichette;
    }

    // 3) Precedenti
    {  //TODO: nel caso di lista {errore, erroree, errori} se si cerca 'erroree' questo algoritmo salta 'errore'
        std::string base = parolaPulita;
        for (int i = static_cast<int>(percorso.size()) - 1; i >= 0 && static_cast<int>(precedenti.size()) < down; --i) {
            base.pop_back();
            char start = static_cast<char>(percorso[i].second - (i == static_cast<int>(percorso.size()) - 1 ? 1 : 0));
            for (char c = start; c >= 'a' && static_cast<int>(precedenti.size()) < down; --c) {
                Lettera* figlio = percorso[i].first->getFiglio(c);
                if (figlio) {
                    std::string temp = base;
                    temp.push_back(c);
                    if (c != start || i == static_cast<int>(percorso.size()) - 1)
                        completaParolaDaNodoPrecedenteMulti(figlio, temp, 'z', precedenti, down);
                    else if (figlio->fineParola) {
                        precedenti.push_back((struct parola){temp, figlio->etichette});
                    }
                }
                if (c == 'a') break;  // evita underflow
            }
        }
        // Ordiniamo lessicograficamente TUTTO il vettore
        std::sort(precedenti.begin(), precedenti.end(),
                  [](const struct parola& a, const struct parola& b) {
                      return a.voce < b.voce;
                  });

    }

    // 4) Successivi
    {
        // figli diretti
        for (char c = 'a'; c <= 'z' && static_cast<int>(successivi.size()) < up; ++c) {
            Lettera* figlio = corrente->getFiglio(c);
            if (figlio) {
                std::string temp = parolaPulita;
                temp.push_back(c);
                completaParolaDaNodoMulti(figlio, temp, 'a', successivi, up);
            }
        }
        // risalita nel percorso
        std::string base = parolaPulita;
        for (int i = static_cast<int>(percorso.size()) - 1;
             i >= 0 && static_cast<int>(successivi.size()) < up; --i) {
            base.pop_back();
            char start = static_cast<char>(percorso[i].second + 1);
            for (char c = start; c <= 'z' && static_cast<int>(successivi.size()) < up; ++c) {
                Lettera* figlio = percorso[i].first->getFiglio(c);
                if (figlio) {
                    std::string temp = base;
                    temp.push_back(c);
                    completaParolaDaNodoMulti(figlio, temp, 'a', successivi, up);
                }
            }
        }
    }

    // 5) Unione finale
    std::vector<struct parola> result;
    result.reserve(precedenti.size() + (!centrale.voce.empty()) + successivi.size());
    result.insert(result.end(), precedenti.begin(), precedenti.end());
    if (!centrale.voce.empty()) result.push_back(centrale);
    result.insert(result.end(), successivi.begin(), successivi.end());

    //DEBUG
    if (true) {
        QString dbString;
        for(struct parola parolaX : result) {
            dbString.append(parolaX.voce + ", ");
        }
        qDebug() << "Exit from cercaParoleInRange (" + QString::number(result.size()) + "): " + dbString;

        dbString = "";
        for(struct parola parolaX : precedenti) {
            dbString.append(parolaX.voce + ", ");
        }
        qDebug() << "Precedenti: " + dbString;

        dbString = "";
        for(struct parola parolaX : successivi) {
            dbString.append(parolaX.voce + ", ");
        }
        qDebug() << "Successivi: " + dbString;
    }

    return result;
}

// --------------------------------------------------
// Completa parole verso destra (alfabetico crescente)
// --------------------------------------------------
void Dizionario::completaParolaDaNodoMulti(const Lettera* nodo, std::string& base, const char initialChar, std::vector<parola>& risultati, size_t maxCount) const {
    if (risultati.size() >= maxCount) return;
    if (nodo->fineParola) {
        risultati.push_back((parola){base, nodo->etichette});
        if (risultati.size() >= maxCount) return;
    }
    for (char c = initialChar; c <= 'z' && risultati.size() < maxCount; ++c) {
        Lettera* figlio = nodo->getFiglio(c);
        if (!figlio) continue;
        base.push_back(c);
        completaParolaDaNodoMulti(figlio, base, 'a', risultati, maxCount);
        base.pop_back();
    }
}

// --------------------------------------------------
// Completa parole verso sinistra (alfabetico decrescente)
// --------------------------------------------------
void Dizionario::completaParolaDaNodoPrecedenteMulti(const Lettera* nodo, std::string& base, const char initialChar, std::vector<parola>& risultati, size_t maxCount) const {
    if (risultati.size() >= maxCount) return;

    for (char c = initialChar; c >= 'a' && risultati.size() < maxCount; --c) {
        Lettera* figlio = nodo->getFiglio(c);
        if (!figlio) continue;
        base.push_back(c);
        completaParolaDaNodoPrecedenteMulti(figlio, base, 'z', risultati, maxCount);
        base.pop_back();
        if (c == 'a') break;  // evita underflow
    }

    if (nodo->fineParola && risultati.size() < maxCount) {
        risultati.push_back((parola){base, nodo->etichette});
    }
}
