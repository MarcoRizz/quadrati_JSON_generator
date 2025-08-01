#ifndef MODIFICADIZIONARIO_H
#define MODIFICADIZIONARIO_H

#include <QMainWindow>
#include "dizionario.h"
#include "widget_displayDictionary.h"
#include "mainwindow.h"

namespace Ui {
class ModificaDizionario;
}

class ModificaDizionario : public QMainWindow
{
    Q_OBJECT

public:
    explicit ModificaDizionario(Dizionario* diz, MainWindow *parent = nullptr);
    ~ModificaDizionario();

signals:
    void editorChiuso();

private slots:
    void on_btn_rimuovi_clicked();

    void on_btn_aggiungi_clicked();

    void on_txt_rimuovi_textChanged(const QString &arg1);

    void on_txt_aggiungi_textChanged(const QString &arg1);

    void on_txt_cerca_textChanged(const QString &arg1);

    void on_btn_salva_clicked();

    void closeEvent(QCloseEvent* event);

private:
    Ui::ModificaDizionario *ui;
    Dizionario* dizionario;
    widget_displayDictionary *dictionaryDisplayer;
};

#endif // MODIFICADIZIONARIO_H
