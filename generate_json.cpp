#include "generate_json.h"

#include <algorithm>
#include <fstream>

#include <QDate>
#include <QFile>
#include <QFileInfo>
#include <QFuture>
#include <QFutureWatcher>
#include <QHash>
#include <QTimer>
#include <QRandomGenerator>
#include <QtConcurrentRun>

#include "customgridletter.h"
#include "custommenubutton.h"
#include "mainwindow.h"


#define DIRECTIONS_n 8

// Direzioni: U, UR, R, DR, D, DL, L, UL
const std::pair<int, int> directions[DIRECTIONS_n] = {
    {0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}
};

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
    oldSnapshot.resize(DIM1);
    for (int i = 0; i < DIM1; ++i) {
        gridSnapshot[i].resize(DIM2);
        oldSnapshot[i].resize(DIM2);
    }

    // Fine ricerca -> si attende che l'utente svuoti la coda, poi riparte il loop
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
    if (wordsComputationRunning) {
        mainWindow->logMessage("Generazione già in corso.");
        return -1;
    }

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
        mainWindow->logMessage("Generazione terminata. Salvare le modifiche del dizionario? S/N");
        return;
    }

    mainWindow->logMessage(QString("--- quadrati#%1 ---").arg(jsons_to_elaborate.front()));

    n_words = 0;
    loop = 0;
    retries = 0;

    mainWindow->clearWords();

    startGenerationLoop();
}


void Generate_JSON::startGenerationLoop()
{
    if (mainWindow->isGridCompleted())
    {
        mainWindow->logMessage(QString("Griglia completata in %1 iterazioni.").arg(loop));

        converti_e_scrivi_JSON();
        jsons_to_elaborate.pop();

        startNextJson();
        return;
    }

    if (loop >= MAX_LOOPS)
    {
        ++retries;

        if (retries >= MAX_RETRIES) {
            mainWindow->logMessage(QString("Impossibile completare quadrati#%1 dopo %2 tentativi: generazione interrotta.")
                                       .arg(jsons_to_elaborate.front())
                                       .arg(retries));
            // Svuoto la coda: meglio fermarsi che lasciare buchi nella numerazione dei JSON
            std::queue<int> vuota;
            jsons_to_elaborate.swap(vuota);
            startNextJson();   // stampa il messaggio di chiusura
            return;
        }

        mainWindow->logMessage(QString("Numero massimo di iterazioni raggiunto: riparto da zero (tentativo %1 di %2).")
                                   .arg(retries + 1)
                                   .arg(MAX_RETRIES));
        loop = 0;
        mainWindow->clearWords();
    }

    mainWindow->updateGridColors();

    creazione_grid();

    loop++;

    creazione_words();   // async -> il workflow continua via segnale
}


void Generate_JSON::creazione_grid() {
    /***********************************************************************************
    // CREAZIONE DELLA GRIGLIA
    ***********************************************************************************/
    // Sostituisco solo le lettere che non servono a nessuna parola accettata
    for (int i = 0; i < DIM1; ++i) {
        for (int j = 0; j < DIM2; ++j) {
            if (!mainWindow->isLetterXYUsed(i, j)) {
                // Lettera casuale tra 'a' e 'z': lavoro in minuscolo perché il dizionario
                // usa solo minuscole, converto in CAPS quando scrivo il JSON
                mainWindow->setGridTile(i, j, randomLetter());
            } else {
                // Tessera confermata: la marco come "vecchia" così la ricerca può saltare
                // i percorsi interamente composti da tessere già analizzate
                mainWindow->setTileOld(i, j);
            }
        }
    }

    //stampo la griglia
    QString gridContent;
    for (int j = 0; j < DIM2; ++j) {
        for (int i = 0; i < DIM1; ++i) {
            gridContent += mainWindow->TileChar(i, j).toUpper();
            gridContent += ' ';
        }
        gridContent += "\n"; // Aggiunge una nuova riga dopo ogni riga della griglia
    }
    mainWindow->logMessage("Contenuto di GRID:\n" + gridContent);
}


void Generate_JSON::creazione_words()
{
    /***********************************************************************************
    // RICERCA DELLE PAROLE NELLA GRIGLIA
    ***********************************************************************************/
    if (wordsComputationRunning)
        return;

    wordsComputationRunning = true;

    emit logMessageRequested(tr("Avvio ricerca parole…"));

    // 1. Preparo i dati per il thread (snapshot della griglia e dei flag "tessera vecchia").
    //    Il worker lavora solo su queste copie: nessun accesso alla GUI da thread secondario.
    for (int i = 0; i < DIM1; ++i) {
        for (int j = 0; j < DIM2; ++j) {
            gridSnapshot[i][j] = mainWindow->TileChar(i, j);
            oldSnapshot[i][j] = mainWindow->isTileOld(i, j);
        }
    }

    // 2. Il watcher avvisa quando il lavoro è finito ('this' come parent per la pulizia)
    auto* watcher = new QFutureWatcher<QVector<FoundWord>>(this);

    connect(watcher, &QFutureWatcher<QVector<FoundWord>>::finished, this, [this, watcher]() {
        const QVector<FoundWord> risultati = watcher->result();
        watcher->deleteLater();
        processWords(risultati);
    });

    /* ------------- parte pesante spostata in un thread --------------- */
    QFuture<QVector<FoundWord>> future = QtConcurrent::run([this]() {
        // Variabile LOCALE al thread: nessuna race condition
        QVector<FoundWord> localResults;

        for (int i = 0; i < DIM1; ++i)
            for (int j = 0; j < DIM2; ++j)
                pathFinder.findWords(i, j, localResults);

        return localResults;
    });
    /* ------------- fine del lavoro pesante ----------------------- */

    watcher->setFuture(future);
}


void Generate_JSON::converti_e_scrivi_JSON() {
    /***********************************************************************************
    // Converto tutto in JSON e scrivo il file
    ***********************************************************************************/
    mainWindow->logMessage(QString("------------"));
    mainWindow->logMessage(QString("JSON FINALE:"));

    // converto la griglia in JSON (in maiuscolo, come se lo aspetta il gioco)
    json grid_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            row.push_back(std::string(1, mainWindow->TileChar(i, j).toUpper().toLatin1()));
        }
        grid_json.push_back(row);
    }

    //stampo la griglia
    QString gridContent;
    for (int j = 0; j < DIM2; ++j) {
        for (int i = 0; i < DIM1; ++i) {
            gridContent += mainWindow->TileChar(i, j).toUpper();
            gridContent += ' ';
        }
        gridContent += "\n";
    }
    mainWindow->logMessage("grid:\n" + gridContent);

    // Divido accettate e bonus e le ordino alfabeticamente: QUESTO è l'ordine a cui si
    // riferiscono gli indici di passingLinks e startingLinks
    QVector<CustomMenuButton*> accettate;
    QVector<CustomMenuButton*> bonus;

    const QVector<CustomMenuButton*> listaParole = mainWindow->getAllActiveWords();
    for (CustomMenuButton* parola : listaParole) {
        if (parola->getPercorsi().isEmpty()) {
            // senza un percorso nella griglia finale la parola non è giocabile
            mainWindow->logMessage(QString("Scarto %1: nessun percorso valido nella griglia.").arg(parola->text()));
            continue;
        }
        if (parola->isBonus())
            bonus.append(parola);
        else
            accettate.append(parola);
    }

    auto perTesto = [](CustomMenuButton* a, CustomMenuButton* b) {
        return QString::compare(a->text(), b->text(), Qt::CaseInsensitive) < 0;
    };
    std::sort(accettate.begin(), accettate.end(), perTesto);
    std::sort(bonus.begin(), bonus.end(), perTesto);

    QHash<CustomMenuButton*, int> indiceDiParola;
    for (int i = 0; i < accettate.size(); ++i)
        indiceDiParola.insert(accettate[i], i);

    // Converto le parole in JSON
    mainWindow->logMessage(QString("words: %1 accettate, %2 bonus").arg(accettate.size()).arg(bonus.size()));

    json words_json = json::array();
    for (CustomMenuButton* parola : std::as_const(accettate)) {
        std::string word_i = parola->text().toStdString();
        std::transform(word_i.begin(), word_i.end(), word_i.begin(), ::toupper);
        words_json.push_back(word_i);
    }

    json words_bonus_json = json::array();
    for (CustomMenuButton* parola : std::as_const(bonus)) {
        std::string word_i = parola->text().toStdString();
        std::transform(word_i.begin(), word_i.end(), word_i.begin(), ::toupper);
        words_bonus_json.push_back(word_i);
    }

    // Converto in JSON le parole che passano da ogni tessera
    json passingLinks_json = json::array();
    for (int i = 0; i < DIM1; ++i) {
        json json_row = json::array();
        for (int j = 0; j < DIM2; ++j) {
            const QSet<CustomMenuButton*> paroleConnesse = mainWindow->TileListOfWords(i, j);

            QVector<int> indices;
            indices.reserve(paroleConnesse.size());
            for (CustomMenuButton* btn : paroleConnesse) {
                const int idx = indiceDiParola.value(btn, -1);
                if (idx >= 0)            // ignoro le parole scartate
                    indices.push_back(idx);
            }
            std::sort(indices.begin(), indices.end());  // ordine crescente

            json json_link = json::array();
            for (int idx : std::as_const(indices))
                json_link.push_back(idx);

            json_row.push_back(json_link);
        }
        passingLinks_json.push_back(json_row);
    }

    // Converto in JSON la tessera di partenza di ogni parola accettata
    // (nello stesso ordine di words_json)
    json startingLinks_json = json::array();
    for (CustomMenuButton* parola : std::as_const(accettate)) {
        const QVector<QVector<CustomGridLetter*>> percorsi = parola->getPercorsi();

        // tra i percorsi possibili ne scelgo uno a caso
        const int dado = QRandomGenerator::global()->bounded(percorsi.count());
        const std::pair<int, int> startingTile = mainWindow->getTileIndexes(percorsi.at(dado).at(0));

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
    if (!file.is_open()) {
        mainWindow->logMessage(QString("Errore durante l'apertura del file: %1").arg(QString::fromStdString(json_name)));
        return;
    }

    file << std::setw(2) << data << std::endl;  // Usa std::setw(2) per una formattazione leggibile
    file.close();
    mainWindow->logMessage(QString("Generato JSON %1").arg(QString::fromStdString(json_name)));
}


void Generate_JSON::onModifiedWord(std::string parola, Etichette et)
{
    // Salvo le modifiche alle etichette nel dizionario
    aggiorna_dizionario(parola, et);

    const customButton_destination dest = findDestination(et);
    customButton_destination originalLocation = NotFound;

    mainWindow->findWordInLists(QString::fromStdString(parola), &originalLocation);

    if (dest == originalLocation)
        return;

    switch (dest) {
    case Accepted:
        mainWindow->addWord(QString::fromStdString(parola), et, Accepted);
        mainWindow->logMessage(QString("%1 -> ACCETTATE").arg(QString::fromStdString(parola)));
        break;

    case Bonus:
        mainWindow->addWord(QString::fromStdString(parola), et, Bonus);
        mainWindow->logMessage(QString("%1 -> BONUS").arg(QString::fromStdString(parola)));
        break;

    case Queue:
        mainWindow->addWord(QString::fromStdString(parola), et, Queue);
        mainWindow->logMessage(QString("%1 -> QUEUE").arg(QString::fromStdString(parola)));
        break;

    default:
        qWarning() << "Destinazione non trovata!";
        break;
    }
}


void Generate_JSON::processWords(const QVector<FoundWord>& words)
{
    int nuove = 0;

    for (const FoundWord& fw : words)
    {
        customButton_destination posizione = NotFound;
        CustomMenuButton* btn = mainWindow->findWordInLists(fw.parola, &posizione);

        if (!btn) {
            // parola mai vista in questa griglia: creo il pulsante nella lista giusta
            posizione = findDestination(fw.etichette);
            emit wordFound(fw.parola, fw.etichette, posizione);   // -> MainWindow::addWord

            btn = mainWindow->findWordInLists(fw.parola);
            if (!btn) {
                qWarning() << "Pulsante non creato per la parola" << fw.parola;
                continue;
            }
            ++nuove;
        }

        // Anche una parola già in lista può guadagnare percorsi nuovi quando cambiano
        // le lettere della griglia: per questo i percorsi si aggiungono sempre.
        QVector<CustomGridLetter*> tessere;
        tessere.reserve(fw.percorso.size());
        for (const QPoint& p : fw.percorso)
            tessere.append(mainWindow->getTile(p.x(), p.y()));

        if (btn->addPercorso(tessere) && posizione != Queue)
            mainWindow->assignWordToTiles(btn, tessere);
    }

    n_words += nuove;
    mainWindow->logMessage(QString("Parole: %1 nuove (%2 percorsi trovati)").arg(nuove).arg(words.size()));

    mainWindow->updateGridColors();

    wordsComputationRunning = false;
    applyPendingDictionaryEdits();

    emit wordsComputationFinished();       // <- SOLO QUI
}


void Generate_JSON::waitQueueEmpty()
{
    if (!mainWindow->boxQueueIsEmpty())
    {
        // la coda si svuota a mano: riprovo tra poco senza bloccare la GUI
        QTimer::singleShot(50, this, &Generate_JSON::waitQueueEmpty);
        return;
    }

    mainWindow->updateGridColors();

    startGenerationLoop();   // <- continua il workflow
}


void Generate_JSON::aggiorna_dizionario(const std::string& testo, const Etichette& etichette)
{
    if (wordsComputationRunning) {
        // Il worker sta leggendo il trie: scrivere adesso sarebbe una corsa critica.
        // La modifica viene applicata appena la ricerca è finita.
        pendingDictionaryEdits.push_back({testo, etichette});
        return;
    }

    dizionario.inserisciParola(testo, etichette, true);
}


void Generate_JSON::applyPendingDictionaryEdits()
{
    for (const PendingEdit& edit : std::as_const(pendingDictionaryEdits))
        dizionario.inserisciParola(edit.parola, edit.etichette, true);

    pendingDictionaryEdits.clear();
}


bool Generate_JSON::backupDizionario() const
{
    const QString percorso = QString::fromStdString(dictionary_path_json);
    const QFileInfo info(percorso);

    if (!info.exists())
        return true;   // niente da salvare

    const QString backup = QString("%1/dizionario_backup_%2.json")
                               .arg(info.absolutePath(),
                                    QDate::currentDate().toString("yy.MM.dd"));

    if (QFile::exists(backup))
        return true;   // backup di oggi già presente

    return QFile::copy(percorso, backup);
}


bool Generate_JSON::salvaDizionario()
{
    applyPendingDictionaryEdits();

    if (!backupDizionario())
        mainWindow->logMessage("Attenzione: backup del dizionario non creato.");

    if (!dizionario.salvaInFileCompatto(dictionary_path_json)) {
        mainWindow->logMessage(QString("Errore nel salvataggio del dizionario al percorso: %1")
                                   .arg(QString::fromStdString(dictionary_path_json)));
        return false;
    }

    mainWindow->logMessage(QString("Dizionario salvato correttamente in: %1")
                               .arg(QString::fromStdString(dictionary_path_json)));
    return true;
}


//--------------------------------------------------------------------------------
// RICERCA DELLE PAROLE: una sola DFS guidata dal trie del dizionario

// Costruttore della classe FindPath
Generate_JSON::FindPath::FindPath(Generate_JSON& gen_json) : parent(gen_json) {}


bool Generate_JSON::FindPath::isValid(int x, int y) const {
    return x >= 0 && x < DIM1 && y >= 0 && y < DIM2 && !visited[x][y];
}


void Generate_JSON::FindPath::findWords(int x, int y, QVector<FoundWord>& results)
{
    const Lettera* radice = parent.dizionario.radiceTrie();
    const Lettera* nodo = Dizionario::figlioDi(radice, parent.gridSnapshot[x][y].toLatin1());

    if (!nodo)
        return;   // nessuna parola del dizionario inizia con questa lettera

    esplora(x, y, 0, nodo, true, results);
}


void Generate_JSON::FindPath::esplora(int x, int y, int step, const Lettera* nodo,
                                      bool analyzedPath, QVector<FoundWord>& results)
{
    path[step] = {x, y};
    visited[x][y] = true;

    // un percorso fatto di sole tessere invariate è già stato analizzato nelle
    // iterazioni precedenti: le sue parole sono già nelle liste
    analyzedPath = analyzedPath && parent.oldSnapshot[x][y];

    if (step + 1 >= MIN_WORD_LENGTH && !analyzedPath) {
        if (const auto etichette = Dizionario::etichetteSeParola(nodo))
            returnFinalWord(step + 1, *etichette, results);
    }

    if (step + 1 < DIM1 * DIM2) {
        for (int i = 0; i < DIRECTIONS_n; ++i) {
            const int newX = x + directions[i].first;
            const int newY = y + directions[i].second;

            if (!isValid(newX, newY))
                continue;

            // IL TAGLIO: se nel dizionario non esiste nessuna parola con questo
            // prefisso, tutti i percorsi che proseguono da qui sono inutili
            const Lettera* figlio = Dizionario::figlioDi(nodo, parent.gridSnapshot[newX][newY].toLatin1());
            if (!figlio)
                continue;

            esplora(newX, newY, step + 1, figlio, analyzedPath, results);
        }
    }

    // Backtracking
    visited[x][y] = false;
}


void Generate_JSON::FindPath::returnFinalWord(int pathLength, const Etichette& etichette,
                                              QVector<FoundWord>& results)
{
    FoundWord fw;
    fw.etichette = etichette;
    fw.parola.reserve(pathLength);
    fw.percorso.reserve(pathLength);

    for (int i = 0; i < pathLength; ++i) {
        const int x = path[i].first;
        const int y = path[i].second;

        fw.parola.append(parent.gridSnapshot[x][y]);
        fw.percorso.append(QPoint(x, y));
    }

    results.push_back(fw);
}
