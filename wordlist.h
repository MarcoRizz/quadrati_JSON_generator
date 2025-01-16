#ifndef WORDLIST_H
#define WORDLIST_H

#include <cstring> // Per memcpy
#include <string>

class WordList {
private:
    struct WordEntry {
        std::string word;                      // Parola
        int index;                        // Indice di inserimento
        std::pair<int, int> startingTile; // Casella di partenza

        // Costruttore
        WordEntry(const std::string& w, int idx, std::pair<int, int> start) : word(w), index(idx), startingTile(start) {}
        WordEntry(const std::string& w, int idx) : word(w), index(idx), startingTile(-1, -1) {}

        // Costruttore predefinito
        WordEntry() : word(""), index(-1), startingTile(-1, -1) {}
    };

    WordEntry* array;   // Array dinamico di parole
    int size;           // Numero di elementi attualmente nell'array
    int capacity;       // Capacità massima attuale dell'array

    // Funzione per riallocare memoria quando necessario
    void resize(int new_capacity);

public:
    // Costruttore
    WordList(int initial_capacity = 100);

    // Distruttore
    ~WordList();

    // Aggiunge una nuova parola in ordine alfabetico
    bool add_word(const std::string& word);

    // Aggiunge una tessera di partenza alla parola con indice di inserimento insertIndex
    bool add_startingTile_by_insertion(const std::pair<int, int> startingTile, int insertIndex);

    // Restituisce la dimensione dell'array
    int get_size() const;

    // Restituisce la parola all'indice specificato (in base all'ordine di inserimento)
    std::string get_word_by_insertion(int index) const;

    // Restituisce la parola in base all'ordine alfabetico
    std::string get_word_by_alphabetical(int index) const;

    // Restituisce la Tile di partenza in base all'ordine alfabetico
    std::pair<int, int> get_startingTile_by_alphabetical(int index);

    // Restituisce la Tile di partenza in base all'ordine di inserimento
    std::pair<int, int> get_startingTile_by_insertion(int index);

    // Restituisce l'indice alfabetico dato l'indice di inserimento
    int get_alphabetical_index(int insertion_index) const;

    // Stampa tutte le parole nell'array
    void print() const;

    //resetto l'oggetto
    void clear();
};

#endif
 // WORDLIST_H
