#include "DinArray.h"


DinArray::DinArray(int initial_capacity)
    : size(0), capacity(initial_capacity){

    }

DinArray::~DinArray() {

}

void DinArray::resize(int new_capacity) {
    array.resize(new_capacity);
}

int DinArray:: get_size() {
    return size;
}

void DinArray::add_value(int value) {
    // Se l'array è pieno, rialloca con una capacità maggiore
    if (size == capacity) {
        resize(capacity + 1);
    }
    array[size] = value;
    size++;
}

int DinArray::get_value(int index) {
    return array[index];
}