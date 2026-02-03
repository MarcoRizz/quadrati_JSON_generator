#include "generate_json.h"

#include <fstream>
#include <iostream>
#include <QtConcurrent>
#include <QRandomGenerator>
#include "mainwindow.h"
#include <qapplication.h>


#define DIRECTIONS_n 8

bool visited[DIM1][DIM2] = {false}; // Array di visitati
std::pair<int, int> path[DIM1 * DIM2]; // Array per memorizzare il percorso

// Direzioni: U, UR, R, DR, D, DL, L, UL
const std::pair<int, int> directions[DIRECTIONS_n] = {
    {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

bool isValid(int x, int y) {
    return x >= 0 && x < DIM1 && y >= 0 && y < DIM2 && !visited[x][y];
}

QChar randomLetter()
{
    int v = QRandomGenerator::global()->bounded(26); // 0..25
    return QChar::fromLatin1('a' + v);
}

// Costruttore di Generate_JSON
Generate_JSON::Generate_JSON(MainWindow* mainWindow)
    : pathFinder(*this), // Inizializzazione dell'istanza della classe FindPath
    mainWindow(mainWindow)
{
    //apro il dizionario
    if (!dizionario.caricaDaFileCompatto(dictionary_path_json)) {
        mainWindow->logMessage(QString("Errore nell'apertura del file al percorso: %1").arg(QString::fromStdString(dictionary_path_json)));
    }

    // Altre inizializzazioni se necessarie
}

customButton_destination findDestination(const Etichette et) {
    // Qui sono indicate le Etichette accettate e quelle bonus
    if (et.haUnaEtichetta(Etichette(Etichette::Approvate))) {
        return Accepted;

    } else if (et.haUnaEtichetta(Etichette(Etichette::BonusNome | Etichette::BonusRaro | Etichette::BonusStraniero))) {
        return Bonus;

    } else {
        return Queue;
    }
}

int Generate_JSON::run()
{
    //calcolo jsons_to_elaborate
    mainWindow->calculateFileNumbers(&jsons_to_elaborate);

    if (jsons_to_elaborate.empty()){
        mainWindow->logMessage(QString("Nessun file da generare."));
        return -1;
    }

    //calcolo i singoli json
    while (!jsons_to_elaborate.empty()) {
        n_words_old = 0;

        mainWindow->clearWords();

        loop = 0;

        while (!mainWindow->isGridCompleted()  && loop < MAX_LOOPS) {

            //aggiorno la schermata
            mainWindow->updateGridColors();
            QApplication::processEvents();

            creazione_grid();

            creazione_words();

            creazione_gridLinks();

            loop++;
        }

        //aggiorno la schermata
        QApplication::processEvents();

        if (loop == MAX_LOOPS) {
            std::cerr << "Numero massimo di iterazioni raggiunto, griglia non trovata" << std::endl;

            //riazzero le variabili ad ogni iterazione
            mainWindow->clearWords();

            continue;  //senza jsons_to_elaborate.pop() ripete l'iterazione
        } else {
            mainWindow->logMessage(QString("Loops: %1").arg(loop));
        }

        converti_e_scrivi_JSON();

        //riazzero le variabili ad ogni iterazione
        jsons_to_elaborate.pop(); //elimina il primo elemento
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

void Generate_JSON::creazione_grid() {
    /***********************************************************************************
    // CREAZIONE ARRAY-2D GRID
    ***********************************************************************************/
    // Popoliamo grid con lettere casuali
    for (int i = 0; i < DIM1; ++i) {
        for (int j = 0; j < DIM2; ++j) {
            if (!mainWindow->isLetterXYUsed(i, j)) {
                mainWindow->setGridTile(i, j, randomLetter());  // Lettera casuale tra 'a' e 'z' (lavoro con lettere minuscole perché il dizionario usa solo minuscole. Converto in CAPS quando scrivo il JSON)
            } else {
                mainWindow->setTileOld(i, j);
            }
            QApplication::processEvents();
        }
    }

    //stampo grid[][]
    QString gridContent;
    for (int j = 0; j < DIM2; ++j) {
        for (int i = 0; i < DIM1; ++i) {
            gridContent += mainWindow->TileChar(i, j).toUpper() + ' ';
        }
        gridContent += "\n"; // Aggiunge una nuova riga dopo ogni riga della griglia
    }
    mainWindow->logMessage("Contenuto di GRID:\n" + gridContent);

    //timer_end = std::chrono::high_resolution_clock::now();
    //duration = timer_end - timer_overall_start;
    //mainWindow->logMessage(QString("Array GRID creato - elapsed time: %1 ms").arg(duration.count()));

    //aggiorno la schermata
    QApplication::processEvents();
}

void Generate_JSON::creazione_words() {
    /***********************************************************************************
    // CREAZIONE ARRAY-1D WORDS
    ***********************************************************************************/
    //qui devo calcolare l'elenco di parole trovate all'interno della griglia

    // 1) Segnalo l’inizio
    emit logMessageRequested(tr("Avvio ricerca parole…"));
    /* ------------- parte pesante spostata in un thread --------------- */
    // 2) Lancio il job pesante in background
    QtConcurrent::run([this]{

        #ifdef PATH_MAX_STEPS
        const int maxSteps = PATH_MAX_STEPS;
        #else
        const int maxSteps = DIM1 * DIM2;
        #endif

        for (int path_size = 4; path_size <= maxSteps; ++path_size) {
            for (int i = 0; i < DIM1; ++i) {
                for (int j = 0; j < DIM2; ++j) {
                    pathFinder.findPaths(i, j, 0, path_size);
                }
            }
        }

        /* ------------- fine del lavoro pesante ----------------------- */

        mainWindow->updateGridColors();
        QApplication::processEvents();

        emit logMessageRequested(tr("Fine ricerca parole…"));
        // Avvisa la GUI che abbiamo terminato
        emit wordsComputationFinished();
    });

    //TODO: qui controllo se ho delle parole in sospeso
    if (!mainWindow) {
        throw std::runtime_error("MainWindow non disponibile");
    }

    // 3) Attendo in modo “bloccante” ma reattivo fino al signal
    QEventLoop loop;
    connect(this, &Generate_JSON::wordsComputationFinished,
            &loop, &QEventLoop::quit,
            Qt::QueuedConnection);
    loop.exec();   // rimane qui finché non arriva quit()

    // 4) Ora posso tornare alla tua vecchia logica:
    //    aspetto che l’utente svuoti la boxQueue
    while (!mainWindow->boxQueueIsEmpty()) {
        QApplication::processEvents();
    }

    //TODO: conta di quante nuove parole ho recuperato in questo loop
}

void Generate_JSON::creazione_gridLinks() {
    /***********************************************************************************
    // CREAZIONE ARRAY-3D GRID_LINKS
    ***********************************************************************************/
    //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (e quali possono iniziare)

    auto activeWords = mainWindow->getAllActiveWords();
    for (CustomMenuButton* word : activeWords) {
        for (int i = 0; i < DIM1; ++i) {
            for (int j = 0; j < DIM2; ++j) {
                pathFinder.findWordPaths(i, j, 0, word);
            }
        }
        mainWindow->updateGridColors();
        QApplication::processEvents();
    }

}

void Generate_JSON::converti_e_scrivi_JSON() {
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
            row.push_back(std::string(1, mainWindow->TileChar(i, j).toLower().toLatin1()));  // converto il char in stringa per il JSON (trasformando in CAP letter)
        }
        grid_json.push_back(row);
    }
    //stampo grid[][]
    QString gridContent;
    for (int j = 0; j < DIM2; ++j) {
        for (int i = 0; i < DIM1; ++i) {
            gridContent += QString("%1 ").arg(mainWindow->TileChar(i, j));
        }
        gridContent += "\n"; // Aggiunge una nuova riga dopo ogni riga della griglia
    }
    mainWindow->logMessage(gridContent);

    // Converto words e words_bonus in JSON
    mainWindow->logMessage(QString("words:"));
    json words_json = json::array();
    json words_bonus_json = json::array();

    QVector<CustomMenuButton*> listaParole;
    listaParole = mainWindow->getAllActiveWords();
    for (auto parola : listaParole)
    {
        std::string word_i = parola->text().toStdString();

        if (parola->isBonus())
        {
            std::transform(word_i.begin(), word_i.end(), word_i.begin(), ::toupper); // Converte ogni carattere in maiuscolo
            words_bonus_json.push_back(word_i);
        } else {
            std::transform(word_i.begin(), word_i.end(), word_i.begin(), ::toupper); // Converte ogni carattere in maiuscolo
            words_json.push_back(word_i);
        }
    }

    // Converto passingWords in JSON
    mainWindow->logMessage("\nwords passingLinks:\n");
    json passingLinks_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json json_row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            QSet<CustomMenuButton*> paroleConnesse = mainWindow->TileListOfWords(i, j);
            QVector<int> indices;
            indices.reserve(paroleConnesse.size());

            for (CustomMenuButton* btn : paroleConnesse) {
                int idx = btn->getAlphabeticalIndex();
                if (idx >= 0) {          // ignora valori invalidi
                    indices.push_back(idx);
                }
            }

            std::sort(indices.begin(), indices.end());  // ordine crescente

            json json_link = json::array();
            for (auto i : indices)
            {
                json_link.push_back(i);
            }

            json_row.push_back(json_link);
        }
        passingLinks_json.push_back(json_row);
    }

    // Converti startingLinks in JSON
    mainWindow->logMessage(QString("\nwords startingLinks:\n"));

    json startingLinks_json = json::array();
    for (auto parola : listaParole)
    {
        if (parola->isBonus())
            continue;

        QVector<QVector<CustomGridLetter*>> percorsi = parola->getPercorsi();
        int dado = QRandomGenerator::global()->bounded(percorsi.count());
        std::pair<int, int> startingTile = mainWindow->getTileIndexes(percorsi.at(dado).at(0));

        json json_pair = json::array();
        json_pair.push_back(startingTile.first);
        json_pair.push_back(startingTile.second);
        startingLinks_json.push_back(json_pair);
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
}

void Generate_JSON::onModifiedWord(std::string parola, Etichette et) {
    customButton_destination dest = findDestination(et);

    customButton_destination originalLocation;
    mainWindow->findWordInLists(QString::fromStdString(parola), &originalLocation);

    if (dest!=originalLocation)
    {
        switch (dest) {
        case Accepted:
            mainWindow->addWord(QString::fromStdString(parola), et, Accepted);
            mainWindow->logMessage(QString("#%1->ACETTATE").arg(QString::fromStdString(parola)));

            break;
        case Bonus:
            mainWindow->addWord(QString::fromStdString(parola), et, Bonus);
            mainWindow->logMessage(QString("#%1->BONUS").arg(QString::fromStdString(parola)));

            break;
        case Queue:
            mainWindow->addWord(QString::fromStdString(parola), et, Queue);
            mainWindow->logMessage(QString("#%1->QUEUE").arg(QString::fromStdString(parola)));

            break;
        default:
            qWarning() << "Destinazione non trovata!";
            break;
        }
    }

    // Salvo le modifiche alle etichette nel dizionario
    dizionario.inserisciParola(parola, et, true);

    QApplication::processEvents();
}

void Generate_JSON::aggiorna_dizionario(const std::string& testo, const Etichette& etichette) {
    // Salvo le modifiche alle etichette nel dizionario
    dizionario.inserisciParola(testo, etichette, true);
}


// Costruttore della classe FindPath
Generate_JSON::FindPath::FindPath(Generate_JSON& gen_json) : parent(gen_json) {}

//--------------------------------------------------------------------------------
//ELABORO TUTTI I POSSIBILI PERCORSI NELLA GRIGLIA

void Generate_JSON::FindPath::returnFinalWord(int pathLength) {
    QString parola;
    for (int i = 0; i < pathLength; ++i) {
        parola.append(parent.mainWindow->TileChar(path[i].first, path[i].second));
    }

    //TODO: investigare qui

    if (!parent.mainWindow->findWordInLists(parola))
    {
        auto rispostaDizionario = parent.dizionario.cercaParola(parola.toStdString());

        if (rispostaDizionario)
        {
            std::cout << "Parola: " << parola.toStdString() << " --> etichette: " << rispostaDizionario->printBitmask() << std::endl;

            customButton_destination dest = findDestination(*rispostaDizionario);
            switch (dest) {
            case Accepted:
                emit parent.wordFound(parola, *rispostaDizionario);

                break;
            case Bonus:
                emit parent.wordFound(parola, *rispostaDizionario, Bonus);

                break;
            case Queue:
                emit parent.wordFound(parola, *rispostaDizionario, Queue);

                break;
            default:
                qWarning() << "Destinazione non trovata!";
                break;
            }

            QApplication::processEvents();
        }

    }
}

//cerca tutti i possibili percorsi nella griglia
void Generate_JSON::FindPath::findPaths(int x, int y, int step, int path_size, bool analyzedPath) {
    path[step] = {x, y};
    visited[x][y] = true;
    analyzedPath = analyzedPath && parent.mainWindow->isTileOld(x, y);

    // Se abbiamo raggiunto il numero di passi massimo (e non è un percorso già analizzato), stampiamo il percorso
    if (step + 1 == path_size && !analyzedPath) {
        returnFinalWord(step + 1);
    } else {
        for (int i = 0; i < DIRECTIONS_n; ++i) {
            int newX = x + directions[i].first;
            int newY = y + directions[i].second;
            if (isValid(newX, newY)) {
                findPaths(newX, newY, step + 1, path_size, analyzedPath);
            }
        }
    }

    // Backtracking
    visited[x][y] = false;
}

//--------------------------------------------------------------------------------
//IDENTIFICO I PERCORSI DI UNA PAROLA

//cerca tutti i possibili percorsi di una specifica parola
void Generate_JSON::FindPath::findWordPaths(int x, int y, int step, CustomMenuButton *word) {
    if (parent.mainWindow->TileChar(x, y) == word->text().at(step)) {
        path[step] = {x, y};
        visited[x][y] = true;

        // Se abbiamo raggiunto il numero di passi massimo, stampiamo il percorso
        if (step + 1 == word->text().length()) {
            parent.mainWindow->addPathToWord(word, path);
        } else {
            for (int i = 0; i < DIRECTIONS_n; ++i) {
                int newX = x + directions[i].first;
                int newY = y + directions[i].second;
                if (isValid(newX, newY)) {
                    findWordPaths(newX, newY, step + 1, word);
                }
            }
        }
    }

    // Backtracking
    visited[x][y] = false;
}
