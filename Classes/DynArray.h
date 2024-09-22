#ifndef DYNARRAY_S
#define DYNARRAY_S

#include <vector>

class DynArray {
private:
    std::vector<int> array;  // Array dinamico di interi
    
public:
    // Costruttore
    DynArray(int initial_capacity = 0);

    // Distruttore
    ~DynArray() = default;

    // Fornisce il numero di elementi
    int get_size() const;

    // Aggiunge un elemento
    bool add_value(int value);

    // Fornisce un elemento
    int get_value(int index) const;
};

#endif