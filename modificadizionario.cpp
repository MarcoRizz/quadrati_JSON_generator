#include "modificadizionario.h"
#include "ui_modificadizionario.h"
#include "etichette.h"

ModificaDizionario::ModificaDizionario(Dizionario* diz, MainWindow* parent)
    : QMainWindow(parent), dizionario(diz)
    , ui(new Ui::ModificaDizionario)
    , dictionaryDisplayer(new widget_displayDictionary)
{
    ui->setupUi(this);
    setWindowTitle("Modifica Dizionario");

    //inizializzo widget_displayDictionary
    dictionaryDisplayer->setDizionario(dizionario); // il tuo oggetto dizionario
    MainWindow* mainWin = qobject_cast<MainWindow*>(parent);
    // Prepara i pulsanti e layout (presi dal .ui)
    QVector<CustomMenuButton *> pulsanti;
    for (int i = 1; i <= 21; ++i) {
        CustomMenuButton *btn = ui->dizionarioScrollArea->findChild<CustomMenuButton *>(QString("pushButton_%1").arg(i));
        if (btn) {
            qDebug() << "CustomMenuButton trovato:" << btn->objectName();
            pulsanti.append(btn);
            //collegamento per aggiorare il dizionario con ogni modifica
            bool ook = connect(btn, &CustomMenuButton::parolaModificata, &mainWin->generate_json, &Generate_JSON::aggiorna_dizionario);
            qDebug() << (ook ? "Connect riuscita" : "Connect fallita");
        } else {
            qDebug() << "pushButton_" << i << " non è CustomMenuButton!";
        }
    }

    dictionaryDisplayer->setLayoutAndButtons(ui->verticalLayout_7, pulsanti); //TODO: ui->verticalLayout_7 non funziona (dovrebbe scorrere su e giù allo scorrere della rotella
    //dictionaryDisplayer->setDizionario(&generate_json.dizionario);
    dictionaryDisplayer->displayParola("dizionario");
}

ModificaDizionario::~ModificaDizionario()
{
    delete ui;
}


void ModificaDizionario::on_btn_rimuovi_clicked()
{
    QString parola = ui->txt_rimuovi->text();

    if (parola.isEmpty())
        return;

    std::string next_parola = dizionario->cercaParolaSuccessiva(parola.toStdString());

    if (next_parola == "")
        next_parola = dizionario->cercaParolaPrecedente(parola.toStdString());

    dizionario->rimuoviParola(parola.toStdString());
    dictionaryDisplayer->displayParola(next_parola);;
}


void ModificaDizionario::on_btn_aggiungi_clicked()
{
    QString parola = ui->txt_aggiungi->text();

    if (parola.isEmpty())
        return;

    Etichette etichetta;

    if (ui->cbox_dcomune->checkState())
        etichetta.aggiungiEtichetta(Etichette(Etichette::DizionarioComune));

    if (ui->cbox_dcomiugazioni->checkState())
        etichetta.aggiungiEtichetta(Etichette(Etichette::Coniugazioni));

    if (ui->cbox_approvata->checkState())
        etichetta.aggiungiEtichetta(Etichette(Etichette::Approvate));

    if (ui->cbox_brara->checkState())
        etichetta.aggiungiEtichetta(Etichette(Etichette::BonusRaro));

    if (ui->cbox_bstraniera->checkState())
        etichetta.aggiungiEtichetta(Etichette(Etichette::BonusStraniero));

    if (ui->cbox_bnome->checkState())
        etichetta.aggiungiEtichetta(Etichette(Etichette::BonusNome));

    dizionario->inserisciParola(parola.toStdString(), etichetta, true);

    dictionaryDisplayer->displayParola(parola.toStdString());
}


void ModificaDizionario::on_txt_rimuovi_textChanged(const QString &arg1)
{
    QString parola = ui->txt_rimuovi->text();

    if (parola.isEmpty())
        return;

    dictionaryDisplayer->displayParola(parola.toStdString());
}


void ModificaDizionario::on_txt_aggiungi_textChanged(const QString &arg1)
{
    QString parola = ui->txt_aggiungi->text();

    if (parola.isEmpty())
        return;

    dictionaryDisplayer->displayParola(parola.toStdString());
}


void ModificaDizionario::on_txt_cerca_textChanged(const QString &arg1)
{
    QString parola = ui->txt_cerca->text();

    if (parola.isEmpty())
        return;

    dictionaryDisplayer->displayParola(parola.toStdString());
}

void ModificaDizionario::on_btn_salva_clicked()
{
    MainWindow* mainWin = qobject_cast<MainWindow*>(parent());
    if (mainWin) {
        dizionario->salvaInFileCompatto(mainWin->generate_json.dictionary_path_json);
        qWarning("Salvataggio effettuato con successo");
    } else {
        qWarning("Impossibile castare parent a MainWindow");
    }
}

