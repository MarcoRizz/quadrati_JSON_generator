#include "CustomMenuButton.h"

#include <QWidgetAction>
#include <QPushButton>
#include <QDebug>
#include <QEvent>
#include <QStringList>
#include <QCheckBox>
#include <QMouseEvent>

// Costruttore della PersistentMenu
PersistentMenu::PersistentMenu(QWidget* parent)
    : QMenu(parent)
{
}

// Override per gestire la chiusura del menu
bool PersistentMenu::event(QEvent* e)
{
    if (e->type() == QEvent::WindowDeactivate) {
        this->close();
    }

    return QMenu::event(e);
}

// Costruttore della CustomMenuButton //TODO: impostare mainWindow->highlightTiles(path, parola.length()) al clic sul bottone
CustomMenuButton::CustomMenuButton(const QString& text, const Etichette &et, Generate_JSON* generate_json, QWidget* parent)
    : QPushButton(text, parent), etichette(et), gen_JSON_address(generate_json)
{
    // Crea il menu personalizzato
    menu = new PersistentMenu(this);

    // Crea e aggiungi le voci
    QMap<Etichette::Valore, QString> etichetteDisponibili = {
        {Etichette::DizionarioComune, "Dizionario Comune"},
        {Etichette::Coniugazioni, "Coniugazioni"},
        {Etichette::Approvate, "Approvate"},
        {Etichette::BonusRaro, "Bonus Raro"},
        {Etichette::BonusStraniero, "Bonus Straniero"},
        {Etichette::BonusNome, "Bonus Nome"}
    }; //{key, value}

    int counter = 0;
    for (auto menuProp = etichetteDisponibili.begin(); menuProp != etichetteDisponibili.end(); ++menuProp) {
        QWidgetAction* action = new QWidgetAction(menu);
        QCheckBox* check = new QCheckBox(menuProp.value());
        check->setStyleSheet("QCheckBox { padding: 5px; }");
        check->setChecked(etichette.haUnaEtichetta(Etichette(menuProp.key())));

        // Disabilita i primi due elementi
        if (counter < 2) {
            check->setEnabled(false);
        }

        // Cattura la chiave per valore
        Etichette::Valore key = menuProp.key();
        connect(check, &QCheckBox::toggled, this, [key, this](bool checked) {
            qDebug() << "Proprietà" << key << (checked ? "aggiunta" : "rimossa");
            if (checked) {
                etichette.aggiungiEtichetta(Etichette(key));
            } else {
                etichette.rimuoviEtichetta(Etichette(key));
            }

            qDebug() << "Proprietà modificate in: " << etichette.printBitmask();
        });

        //collego il segnale di chiusura del menu con la funzione da eseguire
        connect(menu, &QMenu::aboutToHide, this, &CustomMenuButton::onMenuClosed);

        action->setDefaultWidget(check);

        menu->addAction(action);
        ++counter;
    }

    // Mostra il menu al clic del pulsante
    connect(this, &QPushButton::clicked, this, [this]() {
        etichette_originale = etichette; // salva lo stato attuale
        menu->exec(this->mapToGlobal(QPoint(0, this->height())));
    });
}

void CustomMenuButton::onMenuClosed() {
    if (!(etichette == etichette_originale)) {
        gen_JSON_address->onModifiedWord(text().toStdString(), etichette);
    }
}
