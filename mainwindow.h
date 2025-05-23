#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "common_enums.h"
#include "generate_JSON.h"
#include "custommenubutton.h"

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

    void setGridTile(int x, int y, QChar letter);

    void setAskWord(const QString& word);
    Etichette getAskResult() const;

    int getSaveDict() const;

    // Funzione dedicata per l'output
    void logMessage(const QString &message);

    void updateGridColors(const std::vector<std::vector<DynArray>>& passingWords); // Funzione per aggiornare i colori della griglia
    void highlightTiles(const std::pair<int, int>* positions, int size); // Evidenzia alcune celle

    //riguarda l'elenco di parole trovate
    void addWord(const QString &word, const Etichette &etichette, customButton_destination dest = Accepted);
    CustomMenuButton* removeWordFromDestination(const QString &word, customButton_destination exclude);
    void clearWords();

    bool boxQueueIsEmpty();

private slots:
    void on_generate_JSON_clicked();

    void on_btnEnd_N_clicked();
    void on_btnEnd_Y_clicked();

    void on_selectDirectoryButton_clicked();

    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);

private:
    Ui::MainWindow *ui;
    int saveDictionary = 0;     // A fine generazione, salva modifiche al dizionario
};
#endif // MAINWINDOW_H
