#ifndef DIZIONARIO_H
#define DIZIONARIO_H

#include <optional>
#include "lettera.h"

struct parola
{
    std::string voce;
    Etichette etichetta;
};

class Dizionario {
private:
    std::unique_ptr<Lettera> radice;

public:
    // Costruttore
    Dizionario();

    bool isLoaded();

    // Metodo per inserire una parola nel dizionario con etichette
    void inserisciParola(const std::string& parola, Etichette etichette = Etichette(Etichette::Nessuna), bool sovrascriviEtichette = false);  //chiamare inserisciParola("parola", Etichette::DizionarioComune | Etichette::BonusAntico) per assegnare più etichette alla parola.

    // Metodo per rimuovere una parola dal dizionario
    bool rimuoviParola(const std::string& parola);

    // Metodo per cercare una parola nel dizionario (opzionalmente con etichetta)
    std::optional<Etichette> cercaParola(const std::string& parola, Etichette etichette = Etichette(Etichette::Tutte), bool OR_tra_etichette = false) const;  //cercaParola("parola", Etichette::BonusAntico | Etichette::BonusPoetico) per filtrare in base alle etichette

    // Metodo per contare il numero di parole nel dizionario
    int contaParole() const;

    // Metodo per contare le parole con una specifica etichetta
    int contaParoleConEtichetta(Etichette etichette, bool OR_tra_etichette = false) const;

    // Metodo per serializzare il dizionario e salvarlo in un file in formato compatto
    bool salvaInFileCompatto(const std::string& percorsoFile) const;

    // Metodo per caricare il dizionario da un file JSON compatto
    bool caricaDaFileCompatto(const std::string& percorsoFile);

    // Cerca la parola successiva a quella indicata
    std::string cercaParolaSuccessiva(const std::string& parola);

    // Cerca la parola precedente a quella indicata
    std::string cercaParolaPrecedente(const std::string& parola);

    // Cerca un range di parole a partire da "parola"
    std::vector<parola> cercaParoleInRange(const std::string& parola, const int up, const int down);

private:
    // Metodo ricorsivo per eliminare una parola
    bool rimuoviParolaRicorsivo(Lettera* nodo, const std::string& parola, int indice);

    // Metodo ricorsivo per contare le parole
    int contaParoleRicorsivo(const Lettera* nodo) const;

    // Metodo ricorsivo per contare le parole con etichette
    int contaParoleConEtichettaRicorsivo(const Lettera* nodo, Etichette etichette, bool OR_tra_etichette) const;

    // Metodo per sostituire lettere accentate con lettere UTC-8
    std::string rimuoviAccenti(const std::string& input) const;

    // Restituisce la prima parola in ordine alfabetico figlia di "base" (quindi che sia un completamento di "base"). Parte da "nodo" a cercare i figli
    bool completaParolaDaNodo(const Lettera* nodo, std::string& base, const char initialChar) const;

    // Restituisce la prima parola in ordine alfabetico A RITROSO figlia di "base" (quindi che sia un completamento di "base"). Parte da "nodo" a cercare i figli
    bool completaParolaDaNodoPrecedente(const Lettera* nodo, std::string& base, const char initialChar) const;

    // Restituisce "maxCount" parole in ordine alfabetico figlie di "base"
    void completaParolaDaNodoMulti(const Lettera* nodo, std::string& base, const char initialChar, std::vector<parola>& risultati, size_t maxCount) const;

    // Restituisce "maxCount" parole in ordine alfabetico INVERSO figlie di "base"
    void completaParolaDaNodoPrecedenteMulti(const Lettera* nodo, std::string& base, const char initialChar, std::vector<parola>& risultati, size_t maxCount) const;
};

#endif // DIZIONARIO_H
