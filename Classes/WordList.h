#ifndef WORDLIST_H
#define WORDLIST_H

#include <iostream>
#include <cstring> // Per memcpy

using String = std::string;

class WordList {
private:
    String* array;   // Array dinamico di stringhe
    int size;        // Numero di elementi attualmente nell'array
    int capacity;    // Capacità massima attuale dell'array

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

    // Restituisce la parola all'indice specificato
    String get_word(int index) const;

    // Stampa tutte le parole nell'array
    void print() const;
};

#endif