#ifndef GENERATE_JSON_H
#define GENERATE_JSON_H

#include <QObject>
#include <QPoint>
#include <QString>
#include <QVector>
#include <queue>
#include <string>
#include <utility>

#include "common_enums.h"
#include "dizionario.h"

#define DIM1 4
#define DIM2 4

#define MIN_WORD_LENGTH 4   // lunghezza minima di una parola valida
#define MAX_LOOPS 10        // iterazioni di completamento della stessa griglia
#define MAX_RETRIES 3       // ripartenze da zero prima di rinunciare al JSON

class MainWindow;        // Forward declaration
class CustomMenuButton;  // Forward declaration
class CustomGridLetter;  // Forward declaration

customButton_destination findDestination(const Etichette et);

struct FoundWord
{
    QString parola;
    Etichette etichette;
    QVector<QPoint> percorso;   // coordinate delle tessere attraversate
};

class Generate_JSON : public QObject {
    Q_OBJECT

public:
    Generate_JSON(MainWindow* mainWindow); // Dichiarazione del costruttore

    Dizionario dizionario;

    int run();
    void onModifiedWord(std::string parola, Etichette et);
    void aggiorna_dizionario(const std::string& testo, const Etichette& etichette);

    // Scrive il dizionario su file (previo backup giornaliero). Chiamata dai pulsanti S/N.
    bool salvaDizionario();

    const std::string dictionary_path_json = R"(C:\Users\mav13\Documents\Qt\Dizionari\dizionario.json)";

signals:
    void wordFound(const QString& parola, Etichette et, customButton_destination dest = Accepted);
    void logMessageRequested(const QString& testo);
    void wordsComputationFinished();

private:
    MainWindow* mainWindow;

    std::queue<int> jsons_to_elaborate;

    // Copie della griglia passate al thread di ricerca: il worker NON tocca la GUI
    QVector<QVector<QChar>> gridSnapshot;   // lettere
    QVector<QVector<bool>> oldSnapshot;     // tessere invariate dall'iterazione precedente

    int loop = 0;        // iterazioni sulla griglia corrente
    int retries = 0;     // ripartenze da zero sul JSON corrente
    int n_words = 0;     // parole accettate trovate per questo JSON
    bool wordsComputationRunning = false;

    // Modifiche alle etichette arrivate mentre il worker sta leggendo il trie:
    // applicarle subito sarebbe una corsa critica sul dizionario.
    struct PendingEdit { std::string parola; Etichette etichette; };
    QVector<PendingEdit> pendingDictionaryEdits;

    void startNextJson();
    void startGenerationLoop();
    void creazione_grid();
    void creazione_words();
    void converti_e_scrivi_JSON();

    void processWords(const QVector<FoundWord>& words);
    void waitQueueEmpty();
    void applyPendingDictionaryEdits();
    bool backupDizionario() const;

    class FindPath {
    public:
        explicit FindPath(Generate_JSON& gen_json); // Costruttore che riceve un riferimento a Generate_JSON

        // Trova tutte le parole del dizionario che partono dalla tessera (x, y)
        void findWords(int x, int y, QVector<FoundWord>& results);

    private:
        Generate_JSON& parent;                 // Riferimento alla classe Generate_JSON
        bool visited[DIM1][DIM2] = {false};    // Tessere già usate dal percorso corrente
        std::pair<int, int> path[DIM1 * DIM2]; // Percorso corrente

        // DFS unica: avanza di un nodo nel trie per ogni tessera e taglia il ramo
        // appena il prefisso non esiste nel dizionario
        void esplora(int x, int y, int step, const Lettera* nodo, bool analyzedPath, QVector<FoundWord>& results);

        void returnFinalWord(int pathLength, const Etichette& etichette, QVector<FoundWord>& results);
        bool isValid(int x, int y) const;
    };

    FindPath pathFinder; // Istanza della classe nidificata
};

#endif // GENERATE_JSON_H
