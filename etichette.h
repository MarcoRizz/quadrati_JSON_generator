#ifndef ETICHETTE_H
#define ETICHETTE_H

#include <cstdint>

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
        BonusNome        = 1u << 5  // 100000
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
};



#endif // ETICHETTE_H
