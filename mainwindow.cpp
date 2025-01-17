#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>
#include <qfiledialog.h>
#include <QMessageBox>
#include <QSettings>


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
    //TODO: da integrare col widget
    switch (x) {
    case 0:
        switch (y) {
        case 0:
            ui->label_00->setText(letter);
            break;

        case 1:
            ui->label_01->setText(letter);
            break;

        case 2:
            ui->label_02->setText(letter);
            break;

        case 3:
            ui->label_03->setText(letter);
            break;

        default:
            break;
        }
        break;

    case 1:
        switch (y) {
        case 0:
            ui->label_10->setText(letter);
            break;

        case 1:
            ui->label_11->setText(letter);
            break;

        case 2:
            ui->label_12->setText(letter);
            break;

        case 3:
            ui->label_13->setText(letter);
            break;

        default:
            break;
        }
        break;

    case 2:
        switch (y) {
        case 0:
            ui->label_20->setText(letter);
            break;

        case 1:
            ui->label_21->setText(letter);
            break;

        case 2:
            ui->label_22->setText(letter);
            break;

        case 3:
            ui->label_23->setText(letter);
            break;

        default:
            break;
        }
        break;

    case 3:
        switch (y) {
        case 0:
            ui->label_30->setText(letter);
            break;

        case 1:
            ui->label_31->setText(letter);
            break;

        case 2:
            ui->label_32->setText(letter);
            break;

        case 3:
            ui->label_33->setText(letter);
            break;

        default:
            break;
        }
        break;

    default:
        break;
    }
}

void MainWindow::setAskWord(const QString& word)
{
    askResult = Labels::Nessuna;
    currentWord = word;
    ui->label_word->setText("Word: " + currentWord);
}

Labels MainWindow::getAskResult() const
{
    return askResult;
}

void MainWindow::on_btn_accept_clicked()
{
    askResult = Labels::Approvate;
}

void MainWindow::on_btn_bonus_rare_clicked()
{
    askResult = Labels::BonusRaro;
}

void MainWindow::on_btn_bonus_name_clicked()
{
    askResult = Labels::BonusNome;
}

void MainWindow::on_btn_bonus_foreign_clicked()
{
    askResult = Labels::BonusStraniero;
}

void MainWindow::on_btn_reject_clicked()
{
    askResult = Labels::Nessuna;  //TODO
}

void MainWindow::on_btn_google_clicked()
{
    QDesktopServices::openUrl(QUrl("https://www.google.com/search?q=" + currentWord));
}

void MainWindow::on_generate_JSON_clicked()
{
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

