#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "common_enums.h"
#include "generate_JSON.h"

class Generate_JSON;
class CustomMenuButton;
class widget_displayDictionary;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    QString m_selectedDirectory;
    Generate_JSON generate_json;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void calculateFileNumbers(std::queue<int>* list);

    void setAskWord(const QString& word);
    Etichette getAskResult() const;

    int getSaveDict() const;

    // Funzione dedicata per l'output
    void logMessage(const QString &message) const;

    //gestione griglia 4x4
    void setGridTile(const int x, const int y, const QChar letter);
    QChar TileChar(const int x, const int y) const;
    std::pair<int, int> getTileIndexes(const CustomGridLetter* tile) const;
    CustomGridLetter* getTile(const int x, const int y) const;
    void setTileOld(const int x, const int y) const;
    void assignWordToTiles(CustomMenuButton* parola, QVector<QPoint> path);
    bool isTileOld(const int x, const int y) const;
    bool isGridCompleted() const;
    bool isLetterXYUsed(const int x, const int y) const;
    QSet<CustomMenuButton*> TileListOfWords(const int x, const int y, bool fromBonus = false) const;
    void updateGridColors(); // Funzione per aggiornare i colori della griglia
    void highlightTiles(const std::pair<int, int>* positions, int size); // Evidenzia alcune celle

    //riguarda l'elenco di parole trovate
    void addWord(const QString &word, const Etichette &etichette, customButton_destination dest = Accepted);
    CustomMenuButton* findWordInLists(const QString& word, customButton_destination* foundIn = nullptr) const;
    CustomMenuButton* removeWordFromOriginalList(const QString &word, customButton_destination exclude);
    QVector<CustomMenuButton*> getAllActiveWords() const;
    int countInList(customButton_destination list);
    void clearWords();

    bool boxQueueIsEmpty();

private slots:
    void on_generate_JSON_clicked();

    void on_btnEnd_N_clicked();
    void on_btnEnd_Y_clicked();

    void on_selectDirectoryButton_clicked();

    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);

    void on_actionModifica_Dizionario_triggered();  // slot collegato alla voce di menu

private:
    Ui::MainWindow *ui;
    widget_displayDictionary *dictionaryDisplayer;
    int saveDictionary = 0;     // A fine generazione, salva modifiche al dizionario
    CustomGridLetter* letterGrid[4][4];

    void insertWordInList(CustomMenuButton* btn, QWidget* list);
    void removeWord(CustomMenuButton* btn);
    void MoveWordIfExist(std::string parola, Etichette et);

    void aggiorna_widget_Dictionary();
};
#endif // MAINWINDOW_H
