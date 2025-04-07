#include "generate_json.h"

#include <iostream>
#include <fstream>
#include "mainwindow.h"
#include <qapplication.h>

#define DIRECTIONS_n 8

ParolaStatus consulta_dizionario(const std::string& parola);
ParolaStatus ask_the_boss(const std::string& parola);

bool visited[DIM1][DIM2] = {false}; // Array di visitati
std::pair<int, int> path[DIM1 * DIM2]; // Array per memorizzare il percorso

// Direzioni: U, UR, R, DR, D, DL, L, UL
const std::pair<int, int> directions[DIRECTIONS_n] = {
    {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

bool isValid(int x, int y) {
    return x >= 0 && x < DIM1 && y >= 0 && y < DIM2 && !visited[x][y];
}

void Generate_JSON::setAskDialog(std::unique_ptr<MainWindow> dialog) {
    askDialog = std::move(dialog);
}

// Costruttore di Generate_JSON
Generate_JSON::Generate_JSON(MainWindow* mainWindow)
    : passingWords(DIM1, std::vector<DynArray>(DIM2, DynArray(words.get_size()))),
    pathFinder(*this), // Inizializzazione dell'istanza della classe FindPath
    mainWindow(mainWindow)
{
    // Altre inizializzazioni se necessarie
}

int Generate_JSON::run() {
    /*words.add_word("auguri");
    words.add_word( "marcelo");
    passingWords[0][0].add_value(0);
    passingWords[0][1].add_value(0);
    passingWords[0][2].add_value(0);
    passingWords[0][3].add_value(0);
    passingWords[1][3].add_value(0);
    passingWords[2][3].add_value(0);

    passingWords[3][3].add_value(1);
    passingWords[3][2].add_value(1);
    passingWords[3][1].add_value(1);
    passingWords[3][0].add_value(1);
    passingWords[2][0].add_value(1);
    passingWords[1][0].add_value(1);
    passingWords[1][1].add_value(1);*/

    // Inizializza il seme del generatore di numeri casuali
    auto timer_overall_start = std::chrono::high_resolution_clock::now();
    std::srand(std::time(0));

    //apro il dizionario
    if (!dizionario.caricaDaFileCompatto(dictionary_path_json)) {
        mainWindow->logMessage(QString("Errore nell'apertura del file al percorso: %1").arg(QString::fromStdString(dictionary_path_json)));
        return -1;
    }

    auto timer_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = timer_end - timer_overall_start;
    mainWindow->logMessage(QString("Dizionario caricato - elapsed time: %1 ms").arg(duration.count()));

    //calcolo jsons_to_elaborate
    mainWindow->calculateFileNumbers(&jsons_to_elaborate);

    if (jsons_to_elaborate.empty()){
        mainWindow->logMessage(QString("Nessun file da generare."));
        return -1;
    }

    //calcolo i singoli json
    while (!jsons_to_elaborate.empty()) {
        n_words_old = 0;
        n_paths_old = 0;
        n_paths = 0;

        bool completed_grid = false;
        int loop = 0;

        while (!completed_grid  && loop < MAX_LOOPS) {

            //aggiorno la schermata
            mainWindow->updateGridColors(passingWords);
            mainWindow->clearWords();
            QApplication::processEvents();

            /***********************************************************************************
            // CREAZIONE ARRAY-2D GRID
            ***********************************************************************************/
            // Popoliamo grid con lettere casuali
            for (int i = 0; i < DIM1; ++i) {
                for (int j = 0; j < DIM2; ++j) {
                    if (passingWords[i][j].get_size() == 0) {
                        grid[i][j] = 'a' + std::rand() % 26;  // Lettera casuale tra 'a' e 'z' (lavoro con lettere minuscole perché il dizionario usa solo minuscole. Converto in CAPS quando scrivo il JSON)

                        mainWindow->setGridTile(i, j, QChar(grid[i][j]));
                    }
                }
            }

            //stampo grid[][]
            QString gridContent;
            for (int j = 0; j < DIM2; ++j) {
                for (int i = 0; i < DIM1; ++i) {
                    gridContent += QString("%1 ").arg(QChar(grid[i][j] + 'A' - 'a'));
                }
                gridContent += "\n"; // Aggiunge una nuova riga dopo ogni riga della griglia
            }
            mainWindow->logMessage("Contenuto di GRID:\n" + gridContent);

            timer_end = std::chrono::high_resolution_clock::now();
            duration = timer_end - timer_overall_start;
            mainWindow->logMessage(QString("Array GRID creato - elapsed time: %1 ms").arg(duration.count()));

            //aggiorno la schermata
            QApplication::processEvents();

            /***********************************************************************************
            // CREAZIONE ARRAY-1D WORDS
            ***********************************************************************************/
            //qui devo calcolare l'elenco di parole trovate all'interno della griglia

#ifdef PATH_MAX_STEPS
            mainWindow->logMessage(QString("Numero massimo di passi impostato: %1").arg(PATH_MAX_STEPS);
#endif
            for (
#ifdef PATH_MAX_STEPS
                int path_size = 4; path_size <= PATH_MAX_STEPS; ++path_size
#else
                int path_size = 4; path_size <= DIM1 * DIM2; ++path_size
#endif
                ) {
                for (int i = 0; i < DIM1; ++i) {
                    for (int j = 0; j < DIM2; ++j) {
                        pathFinder.findPaths(i, j, 0, path_size);  //qui dentro riempio words
                    }
                }
            }

            timer_end = std::chrono::high_resolution_clock::now();
            duration = timer_end - timer_overall_start;
            mainWindow->logMessage(QString("Array WORDS: %1 nuove parole - elapsed time: %2 ms")
               .arg(words.get_size() - n_words_old)
               .arg(duration.count()));
            n_words_old = words.get_size();

            /***********************************************************************************
            // CREAZIONE ARRAY-3D GRID_LINKS
            ***********************************************************************************/
            //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (e quali possono iniziare)

            for (int word_i = 0; word_i < words.get_size(); ++word_i) {
                std::string running_word = words.get_word_by_insertion(word_i);
                std::pair<int, int> running_start = words.get_startingTile_by_insertion(word_i); //TODO: running_start viene passato a findWordPaths inutilmente, non serve assolutamenet a nulla. Rivedere
                for (int i = 0; i < DIM1; ++i) {
                    for (int j = 0; j < DIM2; ++j) {
                        if(grid[i][j] == running_word[0]) {
                            pathFinder.findWordPaths(i, j, 0, running_word, word_i, running_start);  //qui dentro riempio passingWords
                        }
                    }
                }
                words.add_startingTile_by_insertion(running_start, word_i); // e con questo completo words.startingTile
            }
            mainWindow->updateGridColors(passingWords);
            QApplication::processEvents();

            timer_end = std::chrono::high_resolution_clock::now();
            duration = timer_end - timer_overall_start;
            mainWindow->logMessage(QString("Array PASSINGWORDS: %1 nuovi percorsi - elapsed time: %2 ms")
               .arg(n_paths - n_paths_old)
               .arg(duration.count()));
            n_paths_old = n_paths;
            //se una lettera rimane priva di link, dovrò sostituirla e ripetere il calcolo
            completed_grid = true;
            for (int i = 0; i < DIM1; ++i) {
                for (int j = 0; j < DIM2; ++j) {
                    if (passingWords[i][j].get_size() == 0) {
                        completed_grid = false;
                        break;
                    }
                }
                if (!completed_grid) {
                    mainWindow->logMessage(QString("Elaboro una nuova griglia sostituendo le lettere inutilizzate (iterazione %1)").arg(loop + 1));
                    mainWindow->logMessage(QString("\n"));
                    break;
                }
            }
            loop++;
        }

        //aggiorno la schermata
        QApplication::processEvents();

        if (loop == MAX_LOOPS) {
            std::cerr << "Numero massimo di iterazioni raggiunto, griglia non trovata" << std::endl;

            //riazzero le variabili ad ogni iterazione
            words.clear();
            passingWords.clear();
            passingWords = std::vector<std::vector<DynArray>>(DIM1, std::vector<DynArray>(DIM2, DynArray(words.get_size())));
            words_bonus.clear();

            continue;  //senza jsons_to_elaborate.pop() ripete l'iterazione
        } else {
            mainWindow->logMessage(QString("Loops: %1").arg(loop));
        }

        /***********************************************************************************
        // Converto tutto in JSON e scrivo il file
        ***********************************************************************************/
        mainWindow->logMessage(QString("------------"));
        mainWindow->logMessage(QString("JSON FINALE:"));
        mainWindow->logMessage(QString("grid:"));
        // converto la griglia in JSON
        json grid_json = json::array();
        for (int i = 0; i < DIM1; ++i) {
            json row = json::array();
            for (int j = 0; j < DIM2; ++j) {
                row.push_back(std::string(1, grid[i][j] + 'A' - 'a'));  // converto il char in stringa per il JSON (trasformando in CAP letter)
            }
            grid_json.push_back(row);
        }
        //stampo grid[][]
        QString gridContent;
        for (int j = 0; j < DIM2; ++j) {
            for (int i = 0; i < DIM1; ++i) {
                gridContent += QString("%1 ").arg(grid[i][j]);
            }
            gridContent += "\n"; // Aggiunge una nuova riga dopo ogni riga della griglia
        }
        mainWindow->logMessage(gridContent);

        mainWindow->logMessage(QString("words:"));
        // Converto le parole in JSON
        json words_json = json::array();
        for (int i = 0; i < words.get_size(); ++i) {
            std::string word_i = words.get_word_by_alphabetical(i);
            mainWindow->logMessage(QString("#%1: %2\n").arg(i).arg(QString::fromStdString(word_i)));
            std::transform(word_i.begin(), word_i.end(), word_i.begin(), ::toupper); // Converte ogni carattere in maiuscolo
            words_json.push_back(word_i);
        }

        // Converto passingWords in JSON
        mainWindow->logMessage("\nwords passingLinks:\n");
        json passingLinks_json = json::array();
        for (int i = 0; i < DIM1; ++i) {
            json json_row = json::array();
            for (int j = 0; j < DIM2; ++j) {
                json json_link = json::array();
                int num_links = passingWords[i][j].get_size(); // Ottieni il numero di collegamenti

                mainWindow->logMessage(QString("{%1, %2}").arg(i).arg(j));
                DynArray alphabetical_index;
                for (int k = 0; k < num_links; ++k) {
                    alphabetical_index.add_value(words.get_alphabetical_index(passingWords[i][j].get_value(k)));
                }
                for (int k = 0; k < num_links; ++k) {
                    int value = alphabetical_index.get_value(k);
                    mainWindow->logMessage(QString("%1 ").arg(value));
                    json_link.push_back(value);
                }
                json_row.push_back(json_link);
                mainWindow->logMessage(QString("\n"));
            }
            passingLinks_json.push_back(json_row);
        }

        // Converti startingLinks in JSON
        mainWindow->logMessage(QString("\nwords startingLinks:\n"));
        json startingLinks_json = json::array();
        for (int i = 0; i < words.get_size(); ++i) {
            json json_pair = json::array();
            std::pair<int, int> startingTile = words.get_startingTile_by_alphabetical(i);
            mainWindow->logMessage(QString("#%1 -> {%2, %3}").arg(i).arg(startingTile.first).arg(startingTile.second));
            json_pair.push_back(startingTile.first);
            json_pair.push_back(startingTile.second);
            startingLinks_json.push_back(json_pair);
        }

        // Converto words_bonus in JSON
        mainWindow->logMessage(QString("\nbonus:\n"));
        json words_bonus_json = json::array();
        for (int i = 0; i < words_bonus.get_size(); ++i) {
            std::string word_bonus_i = words_bonus.get_word_by_alphabetical(i);

            bool in_grid = false;
            for (int i = 0; i < DIM1; ++i) {
                for (int j = 0; j < DIM2; ++j) {
                    if (grid[i][j] == word_bonus_i.front()) {  //.front() estrae il primo carattere
                        in_grid = pathFinder.is_still_in_grid(i, j, 0, word_bonus_i);
                        if (in_grid) {
                            break;
                        }
                    }
                }
                if (in_grid) {
                    break;
                }
            }
            if (in_grid) {
                mainWindow->logMessage(QString("#%1: %2").arg(i).arg(QString::fromStdString(word_bonus_i)));
                std::transform(word_bonus_i.begin(), word_bonus_i.end(), word_bonus_i.begin(), ::toupper); // Converte ogni carattere in maiuscolo
                words_bonus_json.push_back(word_bonus_i);
            }
        }

        // creo il contenuto JSON finale
        int todaysNum = jsons_to_elaborate.front();
        json data;
        data["todaysNum"] = todaysNum;
        data["grid"] = grid_json;
        data["words"] = words_json;
        data["passingLinks"] = passingLinks_json;
        data["startingLinks"] = startingLinks_json;
        data["bonus"] = words_bonus_json;

        // Salvare il JSON in un file

        std::string dir = mainWindow->m_selectedDirectory.toStdString();
        std::string json_name = dir + "/quadrati#" + std::to_string(todaysNum) + ".json";

        std::ofstream file(json_name);
        if (file.is_open()) {
            file << std::setw(2) << data << std::endl;  // Usa std::setw(2) per una formattazione leggibile
            file.close();
            mainWindow->logMessage(QString("Generato JSON #%1").arg(QString::fromStdString(json_name)));
        } else {
            throw std::runtime_error("Errore durante l'apertura del file: " + json_name);
        }

        //riazzero le variabili ad ogni iterazione
        jsons_to_elaborate.pop(); //elimina il primo elemento
        words.clear();
        passingWords.clear();
        passingWords = std::vector<std::vector<DynArray>>(DIM1, std::vector<DynArray>(DIM2, DynArray(words.get_size())));
    }

    // chiedo se salvare le modifiche al dizionario
    mainWindow->logMessage(QString("Salvare le modifiche del dizionario? S/N"));

    // Attendiamo l'input dell'utente
    while (mainWindow->getSaveDict() == 0) {
        QApplication::processEvents();
    }

    if (mainWindow->getSaveDict() == 1) {
        if (!dizionario.salvaInFileCompatto(dictionary_path_json)) {
            mainWindow->logMessage(QString("errore nel salvataggio del dizionario al percorso: %1").arg(QString::fromStdString(dictionary_path_json)));
            return -1;
        } else {
            mainWindow->logMessage(QString("Dizionario salvato correttamente in: %1").arg(QString::fromStdString(dictionary_path_json)));
        }
    }

    return 0;
}

// Costruttore della classe FindPath
Generate_JSON::FindPath::FindPath(Generate_JSON& gen_json) : parent(gen_json) {}

//--------------------------------------------------------------------------------
//ELABORO TUTTI I POSSIBILI PERCORSI NELLA GRIGLIA

void Generate_JSON::FindPath::returnFinalWord(int pathLength) {
    std::string parola;
    for (int i = 0; i < pathLength; ++i) {
        parola += parent.grid[path[i].first][path[i].second];
    }
    if (parent.dizionario.cercaParola(parola)) { //se trovo la parola nel json
        //TODO: cercaParola e consulta_dizionario fanno le stesse operazioni --> ottimizzare!
        //consulto i dizionari
        Etichette etichette = consulta_dizionario(parola);

         //TODO: qui indicare le Etichette bonus e le accettate
        if(etichette.haUnaEtichetta(Etichette(Etichette::Approvate))) {

            std::cout << "accettata" << std::endl;
            parent.words.add_word(parola);
            parent.mainWindow->addWord(QString::fromStdString(parola), etichette);
            parent.mainWindow->logMessage(QString("#%1: %2").arg(parent.words.get_size()).arg(QString::fromStdString(parola)));
        } else if(etichette.haUnaEtichetta(Etichette(Etichette::BonusNome | Etichette::BonusRaro | Etichette::BonusStraniero))) {

            std::cout << "bonus" << std::endl;
            parent.words_bonus.add_word(parola);
            parent.mainWindow->addWord(QString::fromStdString(parola), etichette, true);
            parent.mainWindow->logMessage(QString("#%1: %2 - (bonus)").arg(parent.words_bonus.get_size()).arg(QString::fromStdString(parola)));
        } else {
            std::cout << "rimossa" << std::endl;
            parent.dizionario.rimuoviParola(parola);
        }

        QApplication::processEvents();
    }
}

//cerca tutti i possibili percorsi nella griglia
void Generate_JSON::FindPath::findPaths(int x, int y, int step, int path_size) {
    path[step] = {x, y};
    visited[x][y] = true;

    // Se abbiamo raggiunto il numero di passi massimo, stampiamo il percorso
    if (step + 1 == path_size) {
        returnFinalWord(step + 1);
    } else {
        for (int i = 0; i < DIRECTIONS_n; ++i) {
            int newX = x + directions[i].first;
            int newY = y + directions[i].second;
            if (isValid(newX, newY)) {
                findPaths(newX, newY, step + 1, path_size);
            }
        }
    }

    // Backtracking
    visited[x][y] = false;
}

//--------------------------------------------------------------------------------
//IDENTIFICO I PERCORSI DI UNA PAROLA

void Generate_JSON::FindPath::returnFinalPath(int pathLength, int wordIndex, std::pair<int, int>& startingtile) {
    bool nuovoPath = false;
    for (int i = 0; i < pathLength; ++i) {
        nuovoPath |= parent.passingWords[path[i].first][path[i].second].add_value(wordIndex);
        if (i == 0 && startingtile == std::make_pair(-1, -1)) {
            startingtile = path[i];
        }
    }
    if (nuovoPath) {
        parent.mainWindow->logMessage(QString("%1 -> nuovo path: ").arg(QString::fromStdString(parent.words.get_word_by_insertion(wordIndex))));
        for (int i = 0; i < pathLength; ++i) {
            parent.mainWindow->logMessage(QString("{%1, %2}, ").arg(path[i].first).arg(path[i].second));
        }
        parent.mainWindow->logMessage(QString("\n"));
        parent.n_paths++;
    }
}

//cerca tutti i possibili percorsi di una specifica parola
void Generate_JSON::FindPath::findWordPaths(int x, int y, int step, std::string word, int wordIndex, std::pair<int, int>& startingtile) {
    path[step] = {x, y};
    visited[x][y] = true;

    // Se abbiamo raggiunto il numero di passi massimo, stampiamo il percorso
    if (step + 1 == word.size()) {
        returnFinalPath(step + 1, wordIndex, startingtile);
    } else {
        for (int i = 0; i < DIRECTIONS_n; ++i) {
            int newX = x + directions[i].first;
            int newY = y + directions[i].second;
            if (isValid(newX, newY) && parent.grid[newX][newY] == word[step + 1]) {
                findWordPaths(newX, newY, step + 1, word, wordIndex, startingtile);
            }
        }
    }

    // Backtracking
    visited[x][y] = false;
}


//--------------------------------------------------------------------------------
//VALUTO SE LA PAROLA È ANCORA NELLA GRIGLIA

// Cerca il primo percorso possibile per una specifica parola
bool Generate_JSON::FindPath::is_still_in_grid(int x, int y, int step, const std::string& word) {
    // Segna la posizione attuale
    path[step] = {x, y};

    // Se abbiamo raggiunto la lunghezza della parola, stampiamo il percorso
    if (step + 1 == word.size()) {
        return true; // Percorso trovato
    }

    // Esplora le direzioni
    for (int i = 0; i < DIRECTIONS_n; ++i) {
        int newX = x + directions[i].first;
        int newY = y + directions[i].second;

        // Controlla se la nuova posizione è valida, se corrisponde alla lettera successiva della parola
        // e se non corrisponde a uno step precedente di path
        bool isStepPreviouslyVisited = false;
        for (int j = 0; j <= step; ++j) {
            if (path[j] == std::make_pair(newX, newY)) {
                isStepPreviouslyVisited = true;
                break;
            }
        }

        if (isValid(newX, newY) && parent.grid[newX][newY] == word[step + 1] && !isStepPreviouslyVisited) {
            if (is_still_in_grid(newX, newY, step + 1, word)) {
                return true; // Se troviamo un percorso, restituiamo true
            }
        }
    }

    // Non troviamo alcun percorso
    return false;
}


Etichette Generate_JSON::FindPath::consulta_dizionario(const std::string& parola) {

    auto rispostaDizionario = parent.dizionario.cercaParola(parola, Etichette(Etichette::Approvate | Etichette::BonusRaro | Etichette::BonusNome | Etichette::BonusStraniero), true);
    if (rispostaDizionario) {
        std::cout << "Parola: " << parola << " --> etichette: " << rispostaDizionario->printBitmask() << std::endl;
        return *rispostaDizionario;
    }

    return ask_the_boss(parola);
}

Etichette Generate_JSON::FindPath::ask_the_boss(const std::string& parola)
{
    //MainWindow* mainWindow = qobject_cast<MainWindow*>(QApplication::activeWindow());
    MainWindow* mainWindow = parent.mainWindow;

    if (!mainWindow) {
        throw std::runtime_error("MainWindow non disponibile, while elaborating word: " + parola);
    }

    mainWindow->highlightTiles(path, parola.length());
    mainWindow->setAskWord(QString::fromStdString(parola));
    // Attendiamo l'input dell'utente
    while (mainWindow->getAskResult() == Etichette(Etichette::Nessuna)) {
        QApplication::processEvents();
    }

    std::cout << "risposta ottenuta: " << mainWindow->getAskResult().printBitmask() << std::endl;
    parent.dizionario.inserisciParola(parola, mainWindow->getAskResult()); //TODO: ancora non contempla la rimozione
    mainWindow->updateGridColors(parent.passingWords);

    return mainWindow->getAskResult();
}



