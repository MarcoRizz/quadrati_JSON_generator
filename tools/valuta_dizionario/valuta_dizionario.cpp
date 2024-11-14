#include "../../Classes/Dizionario.h"

#define PERCORSO_TREE_DICTIONARY "..\\Dizionari\\dizionario.json"

Dizionario dizionario;

int main() {
     //apro il dizionario
    if (!dizionario.caricaDaFileCompatto(PERCORSO_TREE_DICTIONARY)) {
        std::cout << "errore nell'apertura del file al percorso: " << PERCORSO_TREE_DICTIONARY << std::endl;
        return -1;
    }

    long parole_approvate = dizionario.contaParoleConEtichetta(Labels::Approvate);
    long parole_bonus = dizionario.contaParoleConEtichetta(Labels::BonusNome | Labels::BonusRaro | Labels::BonusStraniero, true);

    long parole_tot = dizionario.contaParole();

    std::cout << "Parole totali: " << parole_tot << std::endl;
    std::cout << "di cui valutate: " << parole_bonus + parole_approvate << " (" << 100.0 * (parole_bonus + parole_approvate) / parole_tot << "%)" << std::endl;

    std::cout << "Parole bonus: " << parole_bonus << " (" << 100.0 * parole_bonus / (parole_bonus + parole_approvate) << "% sulle valutate)" << std::endl;
}