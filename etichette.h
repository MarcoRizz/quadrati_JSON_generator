#ifndef ETICHETTE_H
#define ETICHETTE_H

#include <cstdint>
#include <string>
#include <vector>
#include <sstream>

// Classe per la gestione delle etichette
class Etichette {
public:
    using TipoEtichetta = uint32_t;
    enum Valore : TipoEtichetta {   //(bitmasking):
        Nessuna          = 0,       // 000000
        DizionarioComune = 1u << 0, // 000001
        Coniugazioni     = 1u << 1, // 000010
        Approvate        = 1u << 2, // 000100
        BonusRaro        = 1u << 3, // 001000
        BonusStraniero   = 1u << 4, // 010000
        BonusNome        = 1u << 5,  // 100000
        Tutte            = DizionarioComune | Coniugazioni | Approvate | BonusRaro | BonusStraniero | BonusNome
    };

private:
    TipoEtichetta etichette;

public:
    // Costruttori
    Etichette() : etichette(Nessuna) {}
    explicit Etichette(TipoEtichetta et) : etichette(et) {}

    // Aggiunge un set di etichette
    void aggiungiEtichetta(const Etichette& et) { etichette |= et.etichette; }

    // Rimuove un set di etichette
    void rimuoviEtichetta(const Etichette& et) { etichette &= ~et.etichette; }

    // Controlla se almeno una delle etichette specificate è presente (OR)
    bool haUnaEtichetta(const Etichette& et) const { return (etichette & et.etichette) != 0; }

    // Controlla se tutte le etichette specificate sono presenti (AND)
    bool haTutteEtichette(const Etichette& et) const { return (etichette & et.etichette) == etichette; }

    // Restituisce il valore intero delle etichette
    TipoEtichetta printInt() const { return etichette; }

    // Operatore di uguaglianza per confronto esatto tra due Etichette
    bool operator==(const Etichette& other) const {
        return etichette == other.etichette;
    }

    // Operatore di assegnazione
    Etichette& operator=(const Etichette& other) {
        if (this != &other) {
            etichette = other.etichette;
        }
        return *this;
    }

    // Assegna direttamente un valore TipoEtichetta
    Etichette& operator=(TipoEtichetta valore) {
        etichette = valore;
        return *this;
    }

    std::string printBitmask() const {
        std::vector<std::string> attive;

        if (etichette & DizionarioComune) attive.emplace_back("DizionarioComune");
        if (etichette & Coniugazioni)     attive.emplace_back("Coniugazioni");
        if (etichette & Approvate)        attive.emplace_back("Approvate");
        if (etichette & BonusRaro)        attive.emplace_back("BonusRaro");
        if (etichette & BonusStraniero)   attive.emplace_back("BonusStraniero");
        if (etichette & BonusNome)        attive.emplace_back("BonusNome");

        if (attive.empty()) return "Nessuna";

        std::ostringstream oss;
        for (size_t i = 0; i < attive.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << attive[i];
        }
        return oss.str();
    }
};



#endif // ETICHETTE_H
