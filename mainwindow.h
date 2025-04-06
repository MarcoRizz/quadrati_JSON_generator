#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "generate_json.h"

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
    void addWord(const QString &word, const Etichette &etichette, const bool isBonus = false);
    void clearWords();

private slots:
    // Slots per gestire i pulsanti di "Ask the Boss"
    void on_btn_accept_clicked();
    void on_btn_bonus_rare_clicked();
    void on_btn_bonus_name_clicked();
    void on_btn_bonus_foreign_clicked();
    void on_btn_reject_clicked();
    void on_btn_google_clicked();

    void on_generate_JSON_clicked();

    void on_btnEnd_N_clicked();
    void on_btnEnd_Y_clicked();

    void on_selectDirectoryButton_clicked();

    void on_checkBox_checkStateChanged(const Qt::CheckState &arg1);

private:
    Ui::MainWindow *ui;
    Etichette askResult = Etichette(Etichette::Nessuna); // Risultato dell'azione
    QString currentWord;    // Parola attualmente in valutazione
    int saveDictionary = 0;     // A fine generazione, salva modifiche al dizionario
};
#endif // MAINWINDOW_H
