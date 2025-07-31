#ifndef MODIFICADIZIONARIO_H
#define MODIFICADIZIONARIO_H

#include <QMainWindow>
#include "dizionario.h"
#include "widget_displayDictionary.h"

namespace Ui {
class ModificaDizionario;
}

class ModificaDizionario : public QMainWindow
{
    Q_OBJECT

public:
    explicit ModificaDizionario(Dizionario* diz, QWidget *parent = nullptr);
    ~ModificaDizionario();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_23_clicked();

    void on_pushButton_22_clicked();

    void on_txt_rimuovi_textChanged(const QString &arg1);

    void on_txt_aggiungi_textChanged(const QString &arg1);

    void on_txt_cerca_textChanged(const QString &arg1);

private:
    Ui::ModificaDizionario *ui;
    Dizionario* dizionario;
    widget_displayDictionary *dictionaryDisplayer;
};

#endif // MODIFICADIZIONARIO_H
