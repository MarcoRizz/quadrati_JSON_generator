#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <qfiledialog.h>
#include <QMessageBox>
#include <QSettings>
#include "custommenubutton.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , generate_json(this)
{
    ui->setupUi(this);

    connect(ui->checkBox_JSON_auto_start, &QCheckBox::checkStateChanged, this, &MainWindow::on_checkBox_checkStateChanged);

    // Collegare i pulsanti di "Ask the Boss" ai rispettivi slot
    connect(ui->btn_accept, &QPushButton::clicked, this, &MainWindow::on_btn_accept_clicked);
    connect(ui->btn_bonus_rare, &QPushButton::clicked, this, &MainWindow::on_btn_bonus_rare_clicked);
    connect(ui->btn_bonus_name, &QPushButton::clicked, this, &MainWindow::on_btn_bonus_name_clicked);
    connect(ui->btn_bonus_foreign, &QPushButton::clicked, this, &MainWindow::on_btn_bonus_foreign_clicked);
    connect(ui->btn_reject, &QPushButton::clicked, this, &MainWindow::on_btn_reject_clicked);
    connect(ui->btn_google, &QPushButton::clicked, this, &MainWindow::on_btn_google_clicked);

    // Carica la directory salvata
    QSettings settings("Quadrati", "Quadrati_JSON_generator");
    m_selectedDirectory = settings.value("selectedDirectory", "").toString();
    // Aggiorna la label o altri widget per mostrare la directory salvata
    if (!m_selectedDirectory.isEmpty() && ui->selectedDirectoryLabel) {
        ui->selectedDirectoryLabel->setText(m_selectedDirectory);
    }
}

MainWindow::~MainWindow()
{
    delete ui;
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


void MainWindow::setAskWord(const QString& word)
{
    askResult = Etichette(Etichette::Nessuna);
    currentWord = word;
    ui->label_word->setText("Word: " + currentWord);
}

Etichette MainWindow::getAskResult() const
{
    return askResult;
}

void MainWindow::on_btn_accept_clicked()
{
    askResult = Etichette(Etichette::Approvate);
}

void MainWindow::on_btn_bonus_rare_clicked()
{
    askResult = Etichette(Etichette::BonusRaro);
}

void MainWindow::on_btn_bonus_name_clicked()
{
    askResult = Etichette(Etichette::BonusNome);
}

void MainWindow::on_btn_bonus_foreign_clicked()
{
    askResult = Etichette(Etichette::BonusStraniero);
}

void MainWindow::on_btn_reject_clicked()
{
    askResult = Etichette(Etichette::Nessuna);  //TODO
}

void MainWindow::on_btn_google_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.google.com/search?q=" + currentWord));
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

void MainWindow::addWord(const QString &word, const Etichette &etichette, const bool isBonus) {
    CustomMenuButton *label = new CustomMenuButton(word, etichette);

    // Scegli il layout giusto
    //QScrollArea* layout = isBonus ? ui->boxBonusA : ui->boxAcceptedA;
    QWidget* list = isBonus ? ui->boxBonus : ui->boxAccepted;
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(list->layout());
    if (!layout) {
        qWarning() << "Layout non trovato in boxBonus!";
        return;
    }

    layout->addWidget(label);
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

