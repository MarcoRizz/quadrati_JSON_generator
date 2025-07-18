#include "modificadizionario.h"
#include "ui_modificadizionario.h"

ModificaDizionario::ModificaDizionario(Dizionario* diz, QWidget* parent)
    : QMainWindow(parent), dizionario(diz)
    , ui(new Ui::ModificaDizionario)
{
    ui->setupUi(this);
    setWindowTitle("Modifica Dizionario");
}

ModificaDizionario::~ModificaDizionario()
{
    delete ui;
}
