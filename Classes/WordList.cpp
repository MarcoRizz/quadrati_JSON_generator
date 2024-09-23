#include "WordList.h"


void WordList::resize(int new_capacity) {
    WordEntry* new_array = new WordEntry[new_capacity];
    std::memcpy(new_array, array, size * sizeof(WordEntry));
    delete[] array;
    array = new_array;
    capacity = new_capacity;
}

WordList::WordList(int initial_capacity)
    : size(0), capacity(initial_capacity) {
    array = new WordEntry[capacity];
}

WordList::~WordList() {
    delete[] array;
}

bool WordList::add_word(const String& word) {
    // Trova la posizione in cui inserire la nuova parola
    int insert_pos = 0;
    while (insert_pos < size && array[insert_pos].word < word) {
        insert_pos++;
    }

    if (insert_pos < size && array[insert_pos].word == word) { // Parola già inserita
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

    // Inserisce la nuova parola con l'indice di inserimento
    array[insert_pos] = WordEntry(word, size);

    // Aggiorna la dimensione dell'array
    std::cout << "Parola #" << size++ << ": " << word << std::endl;
    return true;
}

// Restituisce la dimensione dell'array
int WordList::get_size() const {
    return size;
}

// Restituisce la parola all'indice specificato (in base all'ordine di inserimento)
String WordList::get_word_by_insertion(int index) const {
    if (index >= 0 && index < size) {
        for (int i = 0; i < size; ++i) {
            if (array[i].index == index) {
                return array[i].word;
            }
        }
    }
    return ""; // O lancia un'eccezione se preferisci
}

// Restituisce la parola in base all'ordine alfabetico
String WordList::get_word_by_alphabetical(int index) const {
    if (index >= 0 && index < size) {
        return array[index].word; // Stessa logica di accesso
    }
    return ""; // O lancia un'eccezione se preferisci
}

// Implementazione della funzione get_alphabetical_index
int WordList::get_alphabetical_index(int insertion_index) const {
    if (insertion_index < 0 || insertion_index >= size) {
        throw std::out_of_range("Indice di inserimento fuori dall'intervallo");
    }

    // Cerca la posizione della parola in ordine alfabetico
    for (int i = 0; i < size; ++i) {
        if (array[i].index == insertion_index) {
            return i;
        }
    }
    return -1;
}

// Stampa tutte le parole nell'array
void WordList::print() const {
    for (int i = 0; i < size; ++i) {
        std::cout << array[i].word << " (Indice di inserimento: " << array[i].index << ")\n";
    }
}