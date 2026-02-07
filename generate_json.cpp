#include "generate_json.h"

#include <fstream>
#include <iostream>
#include <QtConcurrentRun>    // Per QtConcurrent::run
#include <QFutureWatcher>     // Per gestire il segnale di fine lavoro
#include <QFuture>            // Necessario per tipizzare il watcher (es. QFuture<void>)
#include <qtimer.h>           // per QTimer
#include <QRandomGenerator>
#include "mainwindow.h"
#include <qapplication.h>


#define DIRECTIONS_n 8

// Direzioni: U, UR, R, DR, D, DL, L, UL
const std::pair<int, int> directions[DIRECTIONS_n] = {
    {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

bool Generate_JSON::FindPath::isValid(int x, int y) {
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

    gridSnapshot.resize(DIM1);
    for (int i = 0; i < DIM1; ++i) {
        gridSnapshot[i].resize(DIM2);
    }
    // Altre inizializzazioni se necessarie
    connect(this,
            &Generate_JSON::wordsReady,
            this,
            &Generate_JSON::processWords,
            Qt::QueuedConnection);

    connect(this, &Generate_JSON::wordsComputationFinished,
            this, &Generate_JSON::waitQueueEmpty);

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
    mainWindow->calculateFileNumbers(&jsons_to_elaborate);

    if (jsons_to_elaborate.empty())
    {
        mainWindow->logMessage("Nessun file da generare.");
        return -1;
    }

    startNextJson();

    return 0;
}


void Generate_JSON::startNextJson()
{
    if (jsons_to_elaborate.empty())
    {
        mainWindow->logMessage("Salvare le modifiche del dizionario? S/N");
        return;
    }

    n_words_old = 0;
    loop = 0;

    mainWindow->clearWords();

    startGenerationLoop();
}


void Generate_JSON::startGenerationLoop()
{
    if (mainWindow->isGridCompleted())
    {
        mainWindow->logMessage(QString("Loops: %1").arg(loop));
        qDebug() << "qDebug 1";
        converti_e_scrivi_JSON();
        jsons_to_elaborate.pop();

        startNextJson();
        return;
    }

    if (loop >= MAX_LOOPS)
    {
        std::cerr << "Numero massimo di iterazioni raggiunto\n";
        startNextJson();
        return;
    }

    mainWindow->updateGridColors();

    creazione_grid();

    loop++;

    creazione_words();   // async → continuerà via signal
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

    //aggiorno la schermata
    QApplication::processEvents();
}


void Generate_JSON::creazione_words()
{
    /***********************************************************************************
    // CREAZIONE ARRAY-1D WORDS
    ***********************************************************************************/
    //qui devo calcolare l'elenco di parole trovate all'interno della griglia

    if (wordsComputationRunning)
        return;

    wordsComputationRunning = true;

    emit logMessageRequested(tr("Avvio ricerca parole…"));

    // 1. Prepariamo i dati per il thread (snapshot della griglia)
    for (int i = 0; i < DIM1; ++i)
        for (int j = 0; j < DIM2; ++j)
            gridSnapshot[i][j] = mainWindow->TileChar(i,j);

    // 2. Creiamo il "Watcher" (l'osservatore)
    // Lo creiamo nell'heap (new) con 'this' come parent per la pulizia automatica
    auto* watcher = new QFutureWatcher<QVector<FoundWord>>(this);

    // 3. Connettiamo il segnale: "Quando hai finito, passa i risultati a processWords"
    connect(watcher, &QFutureWatcher<QVector<FoundWord>>::finished, this, [this, watcher]() {
        processWords(watcher->result()); // Passa i risultati ottenuti
        watcher->deleteLater();          // Pulisce il watcher
    });

    /* ------------- parte pesante spostata in un thread --------------- */
    // 4.Avviamo il thread
    QFuture<QVector<FoundWord>> future = QtConcurrent::run([this]() {
        // Variabile LOCALE al thread: nessuna race condition!
        QVector<FoundWord> localResults;
        const int maxSteps = DIM1 * DIM2;

        // Esegui la ricerca (pathFinder deve essere thread-safe o usare dati locali)
        for (int path_size = 4; path_size <= maxSteps; ++path_size)
            for (int i = 0; i < DIM1; ++i)
                for (int j = 0; j < DIM2; ++j)
                    pathFinder.findPaths(i, j, 0, path_size, localResults);
        // Nota: dovresti passare localResults a findPaths per riempirlo

        return localResults;
    });
    /* ------------- fine del lavoro pesante ----------------------- */

    // 5. Diamo il futuro al watcher
    watcher->setFuture(future);

    //TODO: conta di quante nuove parole ho recuperato in questo loop
}


void Generate_JSON::creazione_gridLinks() {
    /***********************************************************************************
    // CREAZIONE ARRAY-3D GRID_LINKS
    ***********************************************************************************/
    //qui devo calcolare tutte le possibilità e calcolare quali parole possono passare da ciascuna lettera (e quali possono iniziare)

    auto activeWords = mainWindow->getAllActiveWords();
    for (CustomMenuButton* word : std::as_const(activeWords)) {
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
    qDebug() << "qDebug 2";
    //stampo grid[][]
    QString gridContent;
    for (int j = 0; j < DIM2; ++j) {
        for (int i = 0; i < DIM1; ++i) {
            gridContent += QString("%1 ").arg(mainWindow->TileChar(i, j));
        }
        gridContent += "\n"; // Aggiunge una nuova riga dopo ogni riga della griglia
    }
    mainWindow->logMessage(gridContent);

    qDebug() << "qDebug 3";
    // Converto words e words_bonus in JSON
    mainWindow->logMessage(QString("words:"));
    json words_json = json::array();
    json words_bonus_json = json::array();

    QVector<CustomMenuButton*> listaParole;
    listaParole = mainWindow->getAllActiveWords();
    for (auto parola : std::as_const(listaParole))
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

    qDebug() << "qDebug 4";
    // Converto passingWords in JSON
    mainWindow->logMessage("\nwords passingLinks:\n");
    json passingLinks_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json json_row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            QSet<CustomMenuButton*> paroleConnesse = mainWindow->TileListOfWords(i, j);
            QVector<int> indices;
            indices.reserve(paroleConnesse.size());

            for (CustomMenuButton* btn : std::as_const(paroleConnesse)) {
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

    qDebug() << "qDebug 5";
    json startingLinks_json = json::array();
    for (auto parola : std::as_const(listaParole))
    {
        if (parola->isBonus())
            continue;

        QVector<QVector<CustomGridLetter*>> percorsi = parola->getPercorsi();

        // CONTROLLO DI SICUREZZA
        if (percorsi.isEmpty()) {
            qWarning() << "Attenzione: la parola" << parola->text() << "non ha percorsi validi!";
            continue; // Salta questa parola per evitare il crash
        }

        int dado = QRandomGenerator::global()->bounded(percorsi.count());

        // Ulteriore controllo per sicurezza sul primo elemento del percorso
        if (percorsi.at(dado).isEmpty()) {
            qWarning() << "Attenzione: il percorso #" << dado << "non è valido";
            continue;
        }

        std::pair<int, int> startingTile = mainWindow->getTileIndexes(percorsi.at(dado).at(0));

        json json_pair = json::array();
        json_pair.push_back(startingTile.first);
        json_pair.push_back(startingTile.second);
        startingLinks_json.push_back(json_pair);
    }

    qDebug() << "qDebug 6";
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

void Generate_JSON::onModifiedWord(std::string parola, Etichette et)
{
    // Salvo le modifiche alle etichette nel dizionario
    dizionario.inserisciParola(parola, et, true);

    customButton_destination dest = findDestination(et);
    customButton_destination originalLocation;

    mainWindow->findWordInLists(QString::fromStdString(parola), &originalLocation);

    if (dest == originalLocation)
        return;

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


void Generate_JSON::processWords(const QVector<FoundWord>& words)
{
    qDebug() << "entro in processWords";
    qDebug() << "Elaborazione di" << words.size() << "parole trovate";

    for (const FoundWord& fw : words)
    {
        customButton_destination dest = findDestination(fw.etichette);

        if (mainWindow->findWordInLists(fw.parola))
        {
            qDebug() << "parola già elaborata: " << fw.parola;
            continue;
        }
        {
            qDebug() << "parola da elaborare: " << fw.parola;

            customButton_destination dest = findDestination(fw.etichette);

            emit wordFound(fw.parola, fw.etichette, dest);
        }

        //TODO: emetto wordFound poi devo lanciare findWordInLists --> spreco di risorse, ottimizzare
        CustomMenuButton* btn = mainWindow->findWordInLists(fw.parola);

        if (!btn)
        {
            qDebug() << "btn fallito";
            continue;
        }

        QVector<CustomGridLetter*> labels;

        for (auto& p : fw.percorso)
            labels.append(mainWindow->getTile(p.x(), p.y()));

        if (btn->addPercorso(labels) && (dest == Accepted or dest == Bonus))
        {
            mainWindow->assignWordToTiles(btn, fw.percorso);
        }
    }

    mainWindow->updateGridColors();
    wordsComputationRunning = false;

    qDebug() << "esco da processWords e avvio waitQueueEmpty";
    emit wordsComputationFinished();       // ← SOLO QUI
}


void Generate_JSON::waitQueueEmpty()
{
    if (!mainWindow->boxQueueIsEmpty())
    {
        QTimer::singleShot(50, this, &Generate_JSON::waitQueueEmpty);
        return;
    }

    mainWindow->updateGridColors();

    qDebug() << "esco da waitQueueEmpty";
    startGenerationLoop();   // ← continua workflow
}



void Generate_JSON::aggiorna_dizionario(const std::string& testo, const Etichette& etichette) {
    // Salvo le modifiche alle etichette nel dizionario
    dizionario.inserisciParola(testo, etichette, true);
}


// Costruttore della classe FindPath
Generate_JSON::FindPath::FindPath(Generate_JSON& gen_json) : parent(gen_json) {}


void Generate_JSON::FindPath::returnFinalWord(int pathLength, QVector<FoundWord>& results)
{
    QString parola;

    QVector<QPoint> percorso;

    for (int i = 0; i < pathLength; ++i) {

        int x = path[i].first;
        int y = path[i].second;

        parola.append(parent.gridSnapshot[x][y]);
        percorso.append(QPoint(x,y));
    }

    auto risposta = parent.dizionario.cercaParola(parola.toStdString());

    if (!risposta)
        return;

    FoundWord fw;
    fw.parola = parola;
    qDebug() << "parola: " << parola;
    fw.etichette = *risposta;
    fw.percorso = percorso;
    for (int i = 0; i < pathLength; ++i) {

        qDebug() << "percorso: " << path[i].first << ", " << path[i].second;
    }

    results.push_back(fw);
}


//cerca tutti i possibili percorsi nella griglia
void Generate_JSON::FindPath::findPaths(int x, int y, int step, int path_size, QVector<FoundWord>& results, bool analyzedPath) {
    path[step] = {x, y};
    visited[x][y] = true;
    analyzedPath = analyzedPath && parent.mainWindow->isTileOld(x, y);

    // Se abbiamo raggiunto il numero di passi massimo (e non è un percorso già analizzato), stampiamo il percorso
    if (step + 1 == path_size) {
        if (!analyzedPath)
            returnFinalWord(step + 1, results);

    } else {
        for (int i = 0; i < DIRECTIONS_n; ++i) {
            int newX = x + directions[i].first;
            int newY = y + directions[i].second;
            if (isValid(newX, newY)) {
                findPaths(newX, newY, step + 1, path_size, results, analyzedPath);
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
    /*if (parent.mainWindow->TileChar(x, y) == word->text().at(step)) {
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
    visited[x][y] = false;*/
}
