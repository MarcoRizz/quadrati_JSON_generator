#ifndef GENERATE_JSON_H
#define GENERATE_JSON_H

#define DIM1 4
#define DIM2 4

#define MAX_LOOPS 10

#include <queue>
#include "wordlist.h"
#include "dizionario.h"
#include "dynarray.h"
#include "common_enums.h"

customButton_destination findDestination(const Etichette et);

class MainWindow; // Forward declaration
class FindPath; // Forward declaration di FindPath

class Generate_JSON {
public:
    Generate_JSON(MainWindow* mainWindow); // Dichiarazione del costruttore

    int run();
    void onModifiedWord(std::string parola, Etichette et);

private:
    MainWindow* mainWindow;

    std::queue<int> jsons_to_elaborate;
    char grid[DIM1][DIM2];
    WordList words;
    WordList words_bonus;
    WordList words_queue;
    Dizionario dizionario;

    const std::string dictionary_path_json = "C:\\Users\\mav13\\Documents\\Qt\\Dizionari\\dizionario.json";
    std::vector<std::vector<DynArray>> passingWords;

    int n_words_old = 0;
    int n_paths_old = 0;
    int n_paths = 0;

    bool completed_grid = false;
    int loop = 0;

    void creazione_grid();
    void creazione_words();
    void creazione_gridLinks();
    void converti_e_scrivi_JSON();

    class FindPath {
    public:
        explicit FindPath(Generate_JSON& gen_json); // Costruttore che riceve un riferimento a Generate_JSON

        void findPaths(int x, int y, int step, int max_size);
        void findWordPaths(int x, int y, int step, std::string word, int wordIndex, std::pair<int, int>& startingtile);
        bool is_still_in_grid(int x, int y, int step, const std::string& word);

    private:
        Generate_JSON& parent; // Riferimento alla classe Generate_JSON

        void returnFinalWord(int pathLength);
        void returnFinalPath(int pathLength, int wordIndex, std::pair<int, int>& startingtile);
    };

    FindPath pathFinder; // Istanza della classe nidificata
};

#endif // GENERATE_JSON_H
