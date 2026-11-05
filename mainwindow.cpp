#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <qfiledialog.h>
#include <QMessageBox>
#include <QSettings>

#include "customgridletter.h"
#include "generate_json.h"
#include "custommenubutton.h"
#include "widget_displayDictionary.h"
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
    for (int x = 0; x < 4; ++x) {
        for (int y = 0; y < 4; ++y) {
            letterGrid[x][y] = findChild<CustomGridLetter*>(QString("label_%1%2").arg(x).arg(y));
            if (!letterGrid[x][y]) {
                qWarning() << "Assegnazione letterGrid fallita";
                return;
            }
        }
    }
    QVector<CustomMenuButton *> pulsanti_dizionario;
    for (int i = 1; i <= 21; ++i) {
        CustomMenuButton *btn = ui->dizionarioScrollArea->findChild<CustomMenuButton *>(QString("pushButton_%1").arg(i));
        if (btn) {
            pulsanti_dizionario.append(btn);
            //collegamento per spostare dalle liste una parola, solo se presente
            bool ok = connect(btn, &CustomMenuButton::parolaModificata, this, &MainWindow::MoveWordIfExist);
            //collegamento per aggiorare il dizionario con ogni modifica
            bool ook = connect(btn, &CustomMenuButton::parolaModificata, &generate_json, &Generate_JSON::aggiorna_dizionario);

        } else {
            qDebug() << "pushButton_" << i << " non è CustomMenuButton!";
        }
    }

    dictionaryDisplayer->setLayoutAndButtons(ui->verticalLayout_7, pulsanti_dizionario); //TODO: ui->verticalLayout_7 non funziona (dovrebbe scorrere su e giù allo scorrere della rotella
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

void MainWindow::on_generate_JSON_clicked()
{
    generate_json.run();
}

void MainWindow::on_btnEnd_N_clicked()
{
    logMessage("Modifiche al dizionario NON salvate.");
}


void MainWindow::on_btnEnd_Y_clicked()
{
    generate_json.salvaDizionario();
}

void MainWindow::logMessage(const QString &message) const
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


void MainWindow::setGridTile(const int x, const int y, const QChar letter)
{
    letterGrid[x][y]->setText(letter);
}


QChar MainWindow::TileChar(const int x, const int y) const
{
    return letterGrid[x][y]->text().at(0);
}


std::pair<int, int> MainWindow::getTileIndexes(const CustomGridLetter* tile) const
{
    std::pair<int, int> indexes;

    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {

            if (tile == letterGrid[r][c])
            {
                indexes.first = r;
                indexes.second = c;
                return indexes;
            }
        }
    }
    indexes.first = -1;
    indexes.second = -1;
    return indexes;
}


CustomGridLetter* MainWindow::getTile(const int x, const int y) const
{
    return letterGrid[x][y];
}


void MainWindow::assignWordToTiles(CustomMenuButton* parola, const QVector<CustomGridLetter*>& percorso)
{
    if (!parola)
        return;

    for (CustomGridLetter* tessera : percorso)
    {
        if (tessera)
            tessera->connectWord(parola, parola->isBonus());
    }
}


bool MainWindow::isGridCompleted() const
{
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {

            if (auto* tessera = letterGrid[r][c]) {

                if (!tessera->isUsed()) return false;
            }
        }
    }

    return true;
}

void MainWindow::setTileOld(const int x, const int y) const
{
    letterGrid[x][y]->setUnchanged(true);
    return;
}

bool MainWindow::isTileOld(const int x, const int y) const
{
    return letterGrid[x][y]->isUnchanged();
}

bool MainWindow::isLetterXYUsed(const int x, const int y) const
{
    return letterGrid[x][y]->isUsed();
}


QSet<CustomMenuButton*> MainWindow::TileListOfWords(const int x, const int y, bool fromBonus) const
{
    return letterGrid[x][y]->UsedBy(fromBonus);
}


void MainWindow::updateGridColors()
{
    for (int r = 0; r < 4; ++r) {
        for (int c = 0; c < 4; ++c) {

            if (CustomGridLetter* tessera = letterGrid[r][c]) {

                if (tessera->isUsed()) {
                    // almeno una parola accettata passa da qui: lettera confermata
                    tessera->setStyleSheet("background-color: green; color: white;");

                } else if (!tessera->UsedBy(true).isEmpty()) {
                    // solo parole bonus: la lettera verrà comunque sostituita
                    tessera->setStyleSheet("background-color: #FFD700; color: black;");

                } else {
                    tessera->setStyleSheet("background-color: lightgrey; color: black;");
                }
            }
        }
    }
}



void MainWindow::addWord(const QString &word, const Etichette &etichette, customButton_destination dest)
{
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
        qWarning() << "Destinazione non valida per la parola" << word;
        return;
    }

    customButton_destination posizione = NotFound;
    CustomMenuButton* label = findWordInLists(word, &posizione);

    if (label && posizione == dest) {
        // già nella lista giusta: aggiorno solo le etichette, i percorsi restano
        label->cambiaParola(word, etichette);
        refreshTileLinks(label, dest);
        return;
    }

    if (label) {
        // sfilo il pulsante dalla lista in cui si trova: i percorsi NON si toccano,
        // cambia solo il ruolo della parola (accettata / bonus / in coda)
        removeWordFromOriginalList(word, dest);
        label->cambiaParola(word, etichette);
    }

    if (!label) {
        label = new CustomMenuButton(word, etichette);

        //collegamento per muovere il bottone tra le liste
        connect(label, &CustomMenuButton::parolaModificata,
                &generate_json, &Generate_JSON::onModifiedWord);
        //collegamento per aggiornare il dizionario ad ogni modifica
        connect(label, &CustomMenuButton::parolaModificata,
                dictionaryDisplayer, &widget_displayDictionary::parolaModificata);
        //collegamento per evideziare ogni parola cliccata
        connect(label, &CustomMenuButton::highLightW,
                dictionaryDisplayer, &widget_displayDictionary::displayParola);
        //collegamento per eliminare il pulsante se rimane senza percorsi
        connect(label, &CustomMenuButton::toEliminate,
                this, &MainWindow::removeWord);
    }

    insertWordInList(label, list);
    refreshTileLinks(label, dest);
}


// Riallinea i collegamenti parola <-> tessere dopo uno spostamento tra liste:
// le parole in coda non colorano la griglia, le bonus non la rendono "completa"
void MainWindow::refreshTileLinks(CustomMenuButton* btn, customButton_destination dest)
{
    if (!btn)
        return;

    const QVector<QVector<CustomGridLetter*>> percorsi = btn->getPercorsi();

    for (const QVector<CustomGridLetter*>& percorso : percorsi) {
        for (CustomGridLetter* tessera : percorso) {
            if (!tessera)
                continue;

            // disconnetto sempre: connectWord non sposta una parola già presente
            // dall'elenco normale a quello bonus (e viceversa)
            tessera->disconnectWord(btn);

            if (dest != Queue)
                tessera->connectWord(btn, dest == Bonus);
        }
    }
}


CustomMenuButton* MainWindow::findWordInLists(
    const QString& word,
    customButton_destination* foundIn /* = nullptr */
    ) const
{
    auto searchInBox = [&](QWidget* box, customButton_destination dest) -> CustomMenuButton* {
        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(box->layout());
        if (!layout)
            return nullptr;

        for (int i = 0; i < layout->count(); ++i) {
            QWidget* w = layout->itemAt(i)->widget();
            if (auto* btn = qobject_cast<CustomMenuButton*>(w)) {

                if (QString::compare(btn->text(), word, Qt::CaseInsensitive) == 0) {
                    if (foundIn)
                        *foundIn = dest;
                    return btn;
                }
            }
        }
        return nullptr;
    };

    if (auto* b = searchInBox(ui->boxAccepted, Accepted)) return b;
    if (auto* b = searchInBox(ui->boxBonus,    Bonus))    return b;
    if (auto* b = searchInBox(ui->boxQueue,    Queue))    return b;

    if (foundIn)
        *foundIn = NotFound;

    return nullptr;
}


// Sfila dal layout il pulsante della parola, cercandolo in tutte le liste tranne
// "exclude". Percorsi e collegamenti alle tessere restano intatti: li riallinea
// refreshTileLinks() quando il pulsante è nella lista di destinazione.
CustomMenuButton* MainWindow::removeWordFromOriginalList(const QString &word, customButton_destination exclude)
{
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


void MainWindow::insertWordInList(CustomMenuButton* btn_new, QWidget* list)
{
    if (!btn_new || !list)
        return;

    QString word = btn_new->text();

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
                layout->insertWidget(i, btn_new);
                inserted = true;
                break;
            }
        }
    }

    if (!inserted) {
        layout->addWidget(btn_new); // Se più grande di tutti, aggiungi in fondo
    }
}


void MainWindow::removeWord(CustomMenuButton* btn)
{
    if (!btn) return;

    // Trova il parent (layout) del pulsante
    QWidget* parentWidget = btn->parentWidget();
    if (!parentWidget) {
        btn->deleteLater(); // se non ha parent, lo eliminiamo comunque in modo sicuro
        return;
    }

    QLayout* layout = parentWidget->layout();
    if (!layout) {
        btn->deleteLater();
        return;
    }

    // Rimuove il widget dal layout
    layout->removeWidget(btn);

    // Opzionale: nascondi prima di cancellare
    btn->hide();

    // Elimina il pulsante in modo sicuro (evita crash se ci sono segnali pendenti)
    btn->deleteLater();
}



// Le etichette modificate dal browser del dizionario valgono anche per la griglia
// in lavorazione, ma solo se quella parola è presente in una delle liste
void MainWindow::MoveWordIfExist(std::string parola, Etichette et)
{
    if (findWordInLists(QString::fromStdString(parola)))
        generate_json.onModifiedWord(parola, et);
}


QVector<CustomMenuButton*> MainWindow::getAllActiveWords() const
{
    QVector<CustomMenuButton*> result;

    auto collectFromBox = [&result](QWidget* box) {
        if (!box) return;

        QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(box->layout());
        if (!layout) return;

        for (int i = 0; i < layout->count(); ++i) {
            QWidget* w = layout->itemAt(i)->widget();
            if (auto* btn = qobject_cast<CustomMenuButton*>(w)) {
                result.push_back(btn);
            }
        }
    };

    collectFromBox(ui->boxAccepted);
    collectFromBox(ui->boxBonus);

    return result;
}


void MainWindow::clearWords()
{
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

    // I pulsanti vengono distrutti subito (non deleteLater): le tessere devono
    // dimenticarli prima del prossimo controllo su isGridCompleted()
    clearLayout(ui->boxAccepted);
    clearLayout(ui->boxBonus);
    clearLayout(ui->boxQueue);
}


bool MainWindow::boxQueueIsEmpty()
{
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
