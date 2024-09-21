#ifndef TILE_H
#define TILE_H

class Tile {
private:
    int* passingWord;  // Array dinamico che contiene l'indice delle parole che PASSANO per questa Tile
    int size1;         // Numero di elementi attualmente nell'array passingWord
    int capacity1;     // Capacità massima attuale dell'array passingWord

    // Funzione per riallocare memoria quando necessario
    void resize1(int new_capacity);

    int* startingWord;  // Array dinamico che contiene l'indice delle parole che INIZIANO da questa Tile
    int size2;          // Numero di elementi attualmente nell'array startingWord
    int capacity2;      // Capacità massima attuale dell'array startingWord

    // Funzione per riallocare memoria quando necessario
    void resize2(int new_capacity);
    
public:
    char lettera; //lettera assegnata e visibile nella griglia grid[][]

    // Costruttore
    Tile(int initial_capacity = 0);

    // Distruttore
    ~Tile();
    
    // Assegno una taglia a entrambi gli array
    void giveSize(int size);
    
    // aggiungo un indice a ciascun array
    void add_passingWord(int value);
    void add_startingWord(int value);

    // fornisco gli indici di ciascun array (TODO)
    //int* get_passingWord();
    //int* get_startingWord();
};

#endif