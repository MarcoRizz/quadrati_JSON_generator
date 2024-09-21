#ifndef DINARRAY_S
#define DINARRAY_S

#include <vector>

class DinArray {
private:
    std::vector<int> array;  // Array dinamico di interi
    int size;                // Numero di elementi attualmente nell'array
    int capacity;            // Capacità massima attuale dell'array

    
public:
    // Costruttore
    DinArray(int initial_capacity = 0);

    // Distruttore
    ~DinArray();
    
    // Funzione per riallocare memoria quando necessario
    void resize(int new_capacity);

    //fornisco il numero di elementi
    int get_size();
    
    // aggiungo un elemento
    void add_value(int value);

    // fornisco un elemento
    int get_value(int index);
};

#endif