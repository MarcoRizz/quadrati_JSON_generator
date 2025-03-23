#include "labelinterattivo.h"

LabelInterattivo::LabelInterattivo(const QString &text, Etichette et, QWidget *parent)
    : QLabel(text, parent), etichette(et) {
    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QLabel::customContextMenuRequested, this, &LabelInterattivo::showMenu);
};


void LabelInterattivo::showMenu(const QPoint &pos) {
    menu.clear();
    QMap<Etichette::Valore, QString> etichetteDisponibili = {
        {Etichette::DizionarioComune, "Dizionario Comune"},
        {Etichette::Coniugazioni, "Coniugazioni"},
        {Etichette::Approvate, "Approvate"},
        {Etichette::BonusRaro, "Bonus Raro"},
        {Etichette::BonusStraniero, "Bonus Straniero"},
        {Etichette::BonusNome, "Bonus Nome"}
    };

    for (auto it = etichetteDisponibili.begin(); it != etichetteDisponibili.end(); ++it) {
        QAction *action = new QAction(it.value(), &menu);
        action->setCheckable(true);
        action->setChecked(etichette.haEtichetta(it.key()));
        connect(action, &QAction::triggered, [this, action, it] {
            if (action->isChecked())
                etichette.aggiungiEtichetta(it.key());
            else
                etichette.rimuoviEtichetta(it.key());
        });
        menu.addAction(action);
    }

    menu.exec(mapToGlobal(pos));
}
