#ifndef WORDLIST_H
#define WORDLIST_H

#include <iostream>
#include <cstring> // Per memcpy

using String = std::string;

class WordList {
private:
    struct WordEntry {
        String word;  // Parola
        int index;    // Indice di inserimento

        // Costruttore
        WordEntry(const String& w, int idx) : word(w), index(idx) {}

        // Costruttore predefinito
        WordEntry() : word(""), index(-1) {}
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
    bool add_word(const String& word);

    // Restituisce la dimensione dell'array
    int get_size() const;

    // Restituisce la parola all'indice specificato (in base all'ordine di inserimento)
    String get_word_by_insertion(int index) const;

    // Restituisce la parola in base all'ordine alfabetico
    String get_word_by_alphabetical(int index) const;

    // Restituisce l'indice alfabetico dato l'indice di inserimento
    int get_alphabetical_index(int insertion_index) const;
    
    // Stampa tutte le parole nell'array
    void print() const;
};

#endif