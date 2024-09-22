#include "DynArray.h"

#include <stdexcept>

DynArray::DynArray(int initial_capacity) {
    array.reserve(initial_capacity);
}

int DynArray:: get_size() const {
    return static_cast<int>(array.size());
}

void DynArray::add_value(int value) {
    // Se l'array non è vuoto e l'ultimo valore è uguale a 'value', non fare nulla
    if (!array.empty() && array.back() == value) {
        return;
    }
    array.push_back(value);
}

int DynArray::get_value(int index) const {
    if (index < 0 || index >= get_size()) {
        throw std::out_of_range("Indice fuori dall'intervallo");
    }
    return array[index];
}