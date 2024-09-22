#include "DynArray.h"

#include <stdexcept>

DynArray::DynArray(int initial_capacity) {
    array.reserve(initial_capacity);
}

int DynArray:: get_size() const {
    return static_cast<int>(array.size());
}

bool DynArray::add_value(int value) {
    // Trova la posizione dove inserire 'value' mantenendo l'ordine crescente
    auto pos = std::lower_bound(array.begin(), array.end(), value);

    // Se il valore è già presente, non fare nulla
    if (pos != array.end() && *pos == value) {
        return false;
    }

    // Inserisce il valore nella posizione corretta
    array.insert(pos, value);
    return true;
}

int DynArray::get_value(int index) const {
    if (index < 0 || index >= get_size()) {
        throw std::out_of_range("Indice fuori dall'intervallo");
    }
    return array[index];
}