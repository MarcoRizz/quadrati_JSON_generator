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

#include "custommenubutton.h"

class Dizionario; // Forward declaration della tua classe Dizionario

class widget_displayDictionary : public QWidget
{
    Q_OBJECT

public:
    explicit widget_displayDictionary(QWidget *parent = nullptr);

    // Inizializza i riferimenti esterni
    void setLayoutAndButtons(QVBoxLayout *layout, const QVector<CustomMenuButton *> &buttons);
    void setDizionario(Dizionario *diz);
    bool displayParola(std::string parola);

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private:
    QVBoxLayout *layoutRef = nullptr;
    QVector<CustomMenuButton *> buttons;
    Dizionario *dizionario = nullptr;

    QString parolaCentrale;  // riferimento alla parola attuale mostrata al centro

    void scorriDizionario(bool avanti); // true = avanti, false = indietro
};


#endif // WIDGET_DISPLAYDICTIONARY_H
