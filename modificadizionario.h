#ifndef MODIFICADIZIONARIO_H
#define MODIFICADIZIONARIO_H

#include <QMainWindow>
#include "dizionario.h"

namespace Ui {
class ModificaDizionario;
}

class ModificaDizionario : public QMainWindow
{
    Q_OBJECT

public:
    explicit ModificaDizionario(Dizionario* diz, QWidget *parent = nullptr);
    ~ModificaDizionario();

private:
    Ui::ModificaDizionario *ui;
    Dizionario* dizionario;
};

#endif // MODIFICADIZIONARIO_H
