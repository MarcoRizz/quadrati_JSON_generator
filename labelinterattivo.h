#ifndef LABELINTERATTIVO_H
#define LABELINTERATTIVO_H

#include <QApplication>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QMap>
#include <QMenu>
#include <QAction>
#include "etichette.h"

// Classe per QLabel interattivo con menu a tendina
class LabelInterattivo : public QLabel {
    Q_OBJECT

public:
    LabelInterattivo(const QString &text, const Etichette &et, QWidget *parent = nullptr);

private:
    Etichette etichette;
    QMenu menu;

    void showMenu(const QPoint &pos);
};


/*// Funzione per creare e assegnare label
void distribuisciLabel(QVBoxLayout* layout1, QVBoxLayout* layout2, const QMap<QString, Etichette>& dati) {
    for (auto it = dati.begin(); it != dati.end(); ++it) {
        LabelInterattivo* label = new LabelInterattivo(it.key(), it.value());
        if (it.value().haEtichetta(Etichette::BonusRaro) || it.value().haEtichetta(Etichette::BonusStraniero) || it.value().haEtichetta(Etichette::BonusNome)) {
            layout2->addWidget(label);
        } else {
            layout1->addWidget(label);
        }
    }
}*/

#endif // LABELINTERATTIVO_H
