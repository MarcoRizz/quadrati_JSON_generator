#ifndef ETICHETTE_H
#define ETICHETTE_H

#include <cstdint>


// Enum per le etichette (bitmasking)
enum Labels : uint32_t {
    Nessuna          = 0,       // 000000
    DizionarioComune = 1u << 0, // 000001
    Coniugazioni     = 1u << 1, // 000010
    Approvate        = 1u << 2, // 000100
    BonusRaro        = 1u << 3, // 001000
    BonusStraniero   = 1u << 4, // 010000
    BonusNome        = 1u << 5  // 100000
};

// Definizione degli operatori di bitmask per Labels
inline Labels operator|(Labels a, Labels b) {
    return static_cast<Labels>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline Labels operator&(Labels a, Labels b) {
    return static_cast<Labels>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

#endif // ETICHETTE_H
