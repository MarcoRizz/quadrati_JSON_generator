#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <qfiledialog.h>
#include <QMessageBox>
#include <QSettings>

#include "modificadizionario.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , generate_json(this)
    , dictionaryDisplayer(new widget_displayDictionary)
{
    ui->setupUi(this);

    // Cliccando su "rileva automaticamente" si disabilita il campo di input manuale
    connect(ui->checkBox_JSON_auto_start, &QCheckBox::checkStateChanged, this, &MainWindow::on_checkBox_checkStateChanged);

    connect(&generate_json, &Generate_JSON::logMessageRequested,
            this, &MainWindow::logMessage);

    connect(&generate_json, &Generate_JSON::wordFound,
            this, &MainWindow::addWord);

    // Carica la directory salvata
    QSettings settings("Quadrati", "Quadrati_JSON_generator");
    m_selectedDirectory = settings.value("selectedDirectory", "").toString();
    // Aggiorna la label o altri widget per mostrare la directory salvata
    if (!m_selectedDirectory.isEmpty() && ui->selectedDirectoryLabel) {
        ui->selectedDirectoryLabel->setText(m_selectedDirectory);
    }

    //inizializzo widget_displayDictionary
    dictionaryDisplayer->setDizionario(&generate_json.dizionario); // il tuo oggetto dizionario
    // Prepara i pulsanti e layout (presi dal .ui)
    QVector<CustomMenuButton *> pulsanti;
    for (int i = 1; i <= 21; ++i) {
        CustomMenuButton *btn = ui->dizionarioScrollArea->findChild<CustomMenuButton *>(QString("pushButton_%1").arg(i));
        if (btn) {
            qDebug() << "CustomMenuButton trovato:" << btn->objectName();
            pulsanti.append(btn);
            //collegamento per spostare dalle liste una parola, solo se presente
            bool ok = connect(btn, &CustomMenuButton::parolaModificata, this, &MainWindow::MoveWordIfExist);
            //collegamento per aggiorare il dizionario con ogni modifica
            bool ook = connect(btn, &CustomMenuButton::parolaModificata, &generate_json, &Generate_JSON::aggiorna_dizionario);
            qDebug() << ((ok & ook) ? "Connect riuscita" : "Connect fallita");

        } else {
            qDebug() << "pushButton_" << i << " non è CustomMenuButton!";
        }
    }

    dictionaryDisplayer->setLayoutAndButtons(ui->verticalLayout_7, pulsanti); //TODO: ui->verticalLayout_7 non funziona (dovrebbe scorrere su e giù allo scorrere della rotella
    //dictionaryDisplayer->setDizionario(&generate_json.dizionario);
    dictionaryDisplayer->displayParola("dizionario");
}

MainWindow::~MainWindow()
{
    delete ui;
    delete dictionaryDisplayer;
}

void MainWindow::calculateFileNumbers(std::queue<int>* list)
{
    int N_start;
    int N_end;

    //N_start manuale o automatico
    if (ui->checkBox_JSON_auto_start->isChecked()) {

        // Controlla se la directory esiste
        QDir dir(m_selectedDirectory);
        if (!dir.exists()) {
            qWarning() << "Directory non trovata:" << m_selectedDirectory;
            return; // Valore di default in caso di errore
        }

        // Filtra per file JSON
        QStringList filters;
        filters << "quadrati#*.json";
        dir.setNameFilters(filters);

        // Ottieni l'elenco dei file
        QStringList fileList = dir.entryList(QDir::Files);

        int maxNumber = 0;

        // Analizza i file
        for (const QString& fileName : fileList) {
            // Cerca la parte del nome che contiene il numero
            int startIdx = fileName.indexOf("quadrati#");
            int endIdx = fileName.indexOf(".json");

            if (startIdx != -1 && endIdx != -1 && endIdx > startIdx) {
                // Estrai la parte numerica tra "quadrati#" e ".json"
                QString numberStr = fileName.mid(startIdx + 9, endIdx - (startIdx + 9));
                bool ok;
                int number = numberStr.toInt(&ok);

                if (ok && number > maxNumber) {
                    maxNumber = number;
                }
            }
        }

        N_start = maxNumber + 1;

    } else {
        bool ok;
        N_start = ui->TextEdit_JSON_start_number->toPlainText().toInt(&ok);

        if (!ok) {
            qWarning() << "Il numero di partenza non è valido";
            return;
        }
    }

    bool ok;
    N_end = N_start + ui->TextEdit_JSON_quantity->toPlainText().toInt(&ok);
    if (!ok || !(N_end > 0)) {
        qWarning() << "La quantità inserita non è valida";
        return;
    }

    for (; N_start < N_end; ++N_start) {
        list->push(N_start);
    }
}

void MainWindow::setGridTile(int x, int y, QChar letter)
{
    QLabel* label = findChild<QLabel*>(QString("label_%1%2").arg(x).arg(y));
    if (label) {
        label->setText(letter);
    }
}

void MainWindow::on_generate_JSON_clicked()
{
    saveDictionary = 0;
    generate_json.run();
}

int MainWindow::getSaveDict() const
{
    return saveDictionary;
}

void MainWindow::on_btnEnd_N_clicked()
{
    saveDictionary = -1;
}


void MainWindow::on_btnEnd_Y_clicked()
{
    saveDictionary = 1;
}

void MainWindow::logMessage(const QString &message)
{
    // Aggiungi il messaggio al QPlainTextEdit
    if (ui->outputTextEdit) {
        ui->outputTextEdit->appendPlainText(message);
    }
}

void MainWindow::on_selectDirectoryButton_clicked()
{
    // Mostra un dialogo di selezione della directory
    QString directory = QFileDialog::getExistingDirectory(this,
                                                          tr("Select Directory"),
                                                          m_selectedDirectory.isEmpty() ? QDir::homePath() : m_selectedDirectory, // Percorso iniziale
                                                          QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

    // Controlla se l'utente ha selezionato una directory
    if (!directory.isEmpty()) {
        // Salva la directory selezionata nella variabile membro
        m_selectedDirectory = directory;

        // Salva la directory nelle impostazioni
        QSettings settings("Quadrati", "Quadrati_JSON_generator");
        settings.setValue("selectedDirectory", m_selectedDirectory);

        ui->selectedDirectoryLabel->setText(m_selectedDirectory);

        // Log per il debug
        qDebug() << "Directory selezionata e salvata: " << m_selectedDirectory;
    } else {
        qDebug() << "Nessuna directory selezionata.";
    }
}

void MainWindow::on_checkBox_checkStateChanged(const Qt::CheckState &arg1)
{
    ui->TextEdit_JSON_start_number->setEnabled(!arg1);
}


void MainWindow::updateGridColors(const std::vector<std::vector<DynArray>>& passingWords) {
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            int val = passingWords[x][y].get_size();
            QLabel* label = findChild<QLabel*>(QString("label_%1%2").arg(x).arg(y));

            if (val == 0)
                label->setStyleSheet("background-color: lightgrey; color: black;");
            else
                label->setStyleSheet("background-color: green; color: white;");
        }
    }
}


void MainWindow::highlightTiles(const std::pair<int, int>* positions, int size) {
    for (int i = 0; i < size; ++i) {
        int x = positions[i].first;
        int y = positions[i].second;
        QLabel* label = findChild<QLabel*>(QString("label_%1%2").arg(x).arg(y));

        if (label) {
            label->setStyleSheet("background-color: yellow; color: black;");
        }
    }
}

void MainWindow::addWord(const QString &word, const Etichette &etichette, customButton_destination dest) {
    CustomMenuButton* label = removeWordFromOriginalList(word, dest);

    if (!label) {
        label = new CustomMenuButton(word, etichette);

        //collegamento per muovere il bottone tra le liste
        connect(label, &CustomMenuButton::parolaModificata, &generate_json, &Generate_JSON::onModifiedWord);
        //collegamento per aggiornare il dizionario ad ogni modifica
        connect(label, &CustomMenuButton::parolaModificata, dictionaryDisplayer, &widget_displayDictionary::parolaModificata);
        //collegamento per evideziare ogni parola cliccata
        connect(label, &CustomMenuButton::highLightW, dictionaryDisplayer, &widget_displayDictionary::displayParola);
    } else {
        label->cambiaParola(word, etichette);
    }

    QWidget* list;
    switch (dest) {
    case Accepted:
        list = ui->boxAccepted;
        break;
    case Bonus:
        list = ui->boxBonus;
        break;
    case Queue:
        list = ui->boxQueue;
        break;
    default:
        qWarning() << "Layout non trovato in boxBonus!";
        return;
    }

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(list->layout());
    if (!layout) {
        qWarning() << "Layout non trovato!";
        return;
    }

    // Inserisci il pulsante in ordine alfabetico
    bool inserted = false;
    for (int i = 0; i < layout->count(); ++i) {
        QWidget* widget = layout->itemAt(i)->widget();
        if (CustomMenuButton* btn = qobject_cast<CustomMenuButton*>(widget)) {
            if (QString::compare(word, btn->text(), Qt::CaseInsensitive) < 0) {
                layout->insertWidget(i, label);
                inserted = true;
                break;
            }
        }
    }

    if (!inserted) {
        layout->addWidget(label); // Se più grande di tutti, aggiungi in fondo
    }
}



CustomMenuButton* MainWindow::removeWordFromOriginalList(const QString &word, customButton_destination exclude) {
    QList<QWidget*> lists = { ui->boxAccepted, ui->boxBonus, ui->boxQueue };

    for (int i = 0; i < lists.size(); ++i) {
        if (static_cast<customButton_destination>(i) == exclude)
            continue;

        QWidget* list = lists[i];
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(list->layout());
        if (!layout)
            continue;

        for (int j = 0; j < layout->count(); ++j) {
            QWidget* widget = layout->itemAt(j)->widget();
            if (CustomMenuButton* btn = qobject_cast<CustomMenuButton*>(widget)) {
                if (btn->text() == word) {
                    layout->removeWidget(btn);
                    //btn->deleteLater(); // se vuoi eliminarlo invece che restituirlo
                    return btn; // restituisci il pulsante rimosso
                }
            }
        }
    }

    return nullptr;
}

void MainWindow::MoveWordIfExist(std::string parola, Etichette et) {
    QList<QWidget*> lists = { ui->boxAccepted, ui->boxBonus, ui->boxQueue };
    QString qparola = QString::fromStdString(parola);

    for (int i = 0; i < lists.size(); ++i) {
        QWidget* list = lists[i];
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(list->layout());
        if (!layout)
            continue;

        for (int j = 0; j < layout->count(); ++j) {
            QWidget* widget = layout->itemAt(j)->widget();
            if (CustomMenuButton* btn = qobject_cast<CustomMenuButton*>(widget)) {
                if (btn->text() == qparola) {
                    generate_json.onModifiedWord(parola, et);
                }
            }
        }
    }

}


void MainWindow::clearWords() {
    auto clearLayout = [](QWidget* list) {
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(list->layout());
        if (!layout) return;
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (QWidget* widget = item->widget()) {
                delete widget;
            }
            delete item;
        }
    };

    clearLayout(ui->boxAccepted);
    clearLayout(ui->boxBonus);
}

bool MainWindow::boxQueueIsEmpty() {
    if (!ui->boxQueue)
        return true;

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui->boxQueue->layout());
    if (!layout)
        return true;

    for (int i = 0; i < layout->count(); ++i) {
        QWidget* widget = layout->itemAt(i)->widget();
        if (qobject_cast<CustomMenuButton*>(widget)) {
            // Trovato almeno un CustomMenuButton
            return false;
        }
    }

    // Nessun CustomMenuButton trovato
    return true;
}


void MainWindow::on_actionModifica_Dizionario_triggered()
{
    auto* editor = new ModificaDizionario(&generate_json.dizionario, this);
    connect(editor, &ModificaDizionario::editorChiuso, this, &MainWindow::aggiorna_widget_Dictionary);
    editor->setAttribute(Qt::WA_DeleteOnClose);
    editor->show();
}


void MainWindow::aggiorna_widget_Dictionary()
{
    QString parola_attuale = ui->pushButton_11->text();
    dictionaryDisplayer->displayParola(parola_attuale.toStdString());
}
