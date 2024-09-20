#include <iostream>
#include <vector>
#include <memory>
#include <string>
#include <unordered_map>

// Definizione della classe Lettera
class Lettera {
public:
    char lettera; // La lettera rappresentata dall'oggetto
    bool fineParola; // Flag per indicare la fine di una parola
    std::unordered_map<char, std::unique_ptr<Lettera>> figli; // Figli dell'oggetto corrente

    // Costruttore
    Lettera(char c = '\0') : lettera(c), fineParola(false) {}

    // Metodo per aggiungere una lettera figlia
    Lettera* aggiungiFiglio(char c) {
        if (figli.find(c) == figli.end()) {
            figli[c] = std::make_unique<Lettera>(c);
        }
        return figli[c].get();
    }

    // Metodo per ottenere un figlio
    Lettera* getFiglio(char c) const {
        auto it = figli.find(c);
        if (it != figli.end()) {
            return it->second.get();
        }
        return nullptr;
    }
};

// Definizione della classe Dizionario
class Dizionario {
private:
    std::unique_ptr<Lettera> radice;

public:
    // Costruttore
    Dizionario() : radice(std::make_unique<Lettera>()) {}

    // Metodo per inserire una parola nel dizionario
    void inserisciParola(const std::string& parola) {
        Lettera* corrente = radice.get();
        for (char c : parola) {
            corrente = corrente->aggiungiFiglio(c);
        }
        corrente->fineParola = true; // Contrassegna la fine della parola
    }

    // Metodo per cercare una parola nel dizionario
    bool cercaParola(const std::string& parola) const {
        Lettera* corrente = radice.get();
        for (char c : parola) {
            corrente = corrente->getFiglio(c);
            if (!corrente) {
                return false; // Lettera non trovata
            }
        }
        return corrente->fineParola; // Verifica se è la fine di una parola
    }

    // Metodo per contare il numero di parole nel dizionario
    int contaParole() const {
        return contaParoleRicorsivo(radice.get());
    }

private:
    // Metodo ricorsivo per contare le parole
    int contaParoleRicorsivo(const Lettera* nodo) const {
        int count = nodo->fineParola ? 1 : 0;
        //PER C++17
        /*for (const auto& [c, figlio] : nodo->figli) {
            count += contaParoleRicorsivo(figlio.get());
        }
        return count;*/
        for (const auto& pair : nodo->figli) {
            const std::unique_ptr<Lettera>& figlio = pair.second;
            count += contaParoleRicorsivo(figlio.get());
        }
        return count;
    }
};

// Funzione main per dimostrare l'uso del Dizionario
int main() {
    Dizionario dizionario;

    // Inserimento di parole nel dizionario
    dizionario.inserisciParola("cane");
    dizionario.inserisciParola("casa");
    dizionario.inserisciParola("caro");
    dizionario.inserisciParola("care");
    dizionario.inserisciParola("cat");
    dizionario.inserisciParola("cane"); // Parola duplicata

    // Ricerca di parole
    std::string paroleDaCercare[] = {"cane", "casa", "caro", "care", "cat", "cate", "can"};
    for (const auto& parola : paroleDaCercare) {
        if (dizionario.cercaParola(parola)) {
            std::cout << "La parola \"" << parola << "\" esiste nel dizionario." << std::endl;
        } else {
            std::cout << "La parola \"" << parola << "\" non esiste nel dizionario." << std::endl;
        }
    }

    // Conteggio delle parole nel dizionario
    int totaleParole = dizionario.contaParole();
    std::cout << "\nNumero totale di parole nel dizionario: " << totaleParole << std::endl;

    return 0;
}
