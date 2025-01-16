#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
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
    Generate_JSON generate_json;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setGridTile(int x, int y, QChar letter);

    void setAskWord(const QString& word);
    Labels getAskResult() const;

    int getSaveDict() const;

    // Funzione dedicata per l'output
    void logMessage(const QString &message);

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

private:
    Ui::MainWindow *ui;
    Labels askResult = Nessuna; // Risultato dell'azione
    QString currentWord;    // Parola attualmente in valutazione
    int saveDictionary = 0;     // A fine generazione, salva modifiche al dizionario
};
#endif // MAINWINDOW_H
