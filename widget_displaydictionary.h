#ifndef WIDGET_DISPLAYDICTIONARY_H
#define WIDGET_DISPLAYDICTIONARY_H

#pragma once

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QEvent>
#include <QWheelEvent>
#include <QVector>
#include <QString>

class Dizionario; // Forward declaration della tua classe Dizionario

class widget_displayDictionary : public QWidget
{
    Q_OBJECT

public:
    explicit widget_displayDictionary(QWidget *parent = nullptr);

    // Inizializza i riferimenti esterni
    void setLayoutAndButtons(QVBoxLayout *layout, const QVector<QPushButton *> &buttons);
    void setDizionario(Dizionario *diz);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QVBoxLayout *layoutRef = nullptr;
    QVector<QPushButton *> buttons;
    Dizionario *dizionario = nullptr;

    QString parolaCentrale;  // riferimento alla parola attuale mostrata al centro

    void scorriDizionario(bool avanti); // true = avanti, false = indietro
    void cercaParola(std::string parola);
};


#endif // WIDGET_DISPLAYDICTIONARY_H
