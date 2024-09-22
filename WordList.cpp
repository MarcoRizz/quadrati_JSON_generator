#include "WordList.h"


void WordList::resize(int new_capacity) {
    String* new_array = new String[new_capacity];
    std::memcpy(new_array, array, size * sizeof(String));
    delete[] array;
    array = new_array;
    capacity = new_capacity;
}

WordList::WordList(int initial_capacity)
    : size(0), capacity(initial_capacity) {
    array = new String[capacity];
}

WordList::~WordList() {
    delete[] array;
}

bool WordList::add_word(const String& word) {
    // Trova la posizione in cui inserire la nuova parola
    int insert_pos = 0;
    while (insert_pos < size && array[insert_pos] < word) {
        insert_pos++;
    }

    if (insert_pos < size && array[insert_pos] == word) { //parola già inserita esco senza modifiche
        return false;
    }
    
    // Se l'array è pieno, rialloca con una capacità maggiore
    if (size == capacity) {
        resize(capacity > 0 ? capacity * 2 : 1);
    }

    // Sposta gli elementi a destra per fare spazio alla nuova parola
    for (int i = size; i > insert_pos; --i) {
        array[i] = array[i - 1];
    }

    // Inserisce la nuova parola
    array[insert_pos] = word;

    // Aggiorna la dimensione dell'array
    std::cout <<"Parola #" << size++ << ": " << word << std::endl;
    return true;
}

// Restituisce la dimensione dell'array
int WordList::get_size() const {
    return size;
}

// Restituisce la parola all'indice specificato
String WordList::get_word(int index) const {
    if (index >= 0 && index < size) {
        return array[index];
    }
    return ""; // O lancia un'eccezione se preferisci
}

// Stampa tutte le parole nell'array
void WordList::print() const {
    for (int i = 0; i < size; ++i) {
        std::cout << array[i] << std::endl;
    }
}