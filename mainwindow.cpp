#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QDesktopServices>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , generate_json(this)
{
    ui->setupUi(this);

    // Collegare i pulsanti di "Ask the Boss" ai rispettivi slot
    connect(ui->btn_accept, &QPushButton::clicked, this, &MainWindow::on_btn_accept_clicked);
    connect(ui->btn_bonus_rare, &QPushButton::clicked, this, &MainWindow::on_btn_bonus_rare_clicked);
    connect(ui->btn_bonus_name, &QPushButton::clicked, this, &MainWindow::on_btn_bonus_name_clicked);
    connect(ui->btn_bonus_foreign, &QPushButton::clicked, this, &MainWindow::on_btn_bonus_foreign_clicked);
    connect(ui->btn_reject, &QPushButton::clicked, this, &MainWindow::on_btn_reject_clicked);
    connect(ui->btn_google, &QPushButton::clicked, this, &MainWindow::on_btn_google_clicked);
}

MainWindow::~MainWindow()
{
    delete ui;
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

void MainWindow::logMessage(const QString &message) {
    // Aggiungi il messaggio al QPlainTextEdit
    if (ui->outputTextEdit) {
        ui->outputTextEdit->appendPlainText(message);
    }
}
