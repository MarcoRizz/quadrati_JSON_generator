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

CustomMenuButton::CustomMenuButton(QWidget* parent)
    : CustomMenuButton("CustomButton", Etichette(), parent)
{}

// Costruttore della CustomMenuButton //TODO: impostare mainWindow->highlightTiles(path, parola.length()) al clic sul bottone
CustomMenuButton::CustomMenuButton(const QString& text, const Etichette &et, QWidget* parent)
    : QPushButton(text, parent), etichette(et)
{
    qDebug() << "New CustomButton:" << text;
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
        emit highLightW(this->text().toStdString());
        menu->exec(this->mapToGlobal(QPoint(0, this->height())));
    });

    // Assegno il colore
    aggiornaColoreSfondo();
}

void CustomMenuButton::cambiaParola(const QString& text, const Etichette &et) {
    setText(text);
    etichette = et;
    etichette_originale = et;

    aggiornaColoreSfondo();

    // Aggiorna i checkbox del menu
    const QList<QAction*> azioni = menu->actions();
    int counter = 0;
    for (QAction* azione : azioni) {
        QWidgetAction* widgetAction = qobject_cast<QWidgetAction*>(azione);
        if (!widgetAction) continue;

        QCheckBox* check = qobject_cast<QCheckBox*>(widgetAction->defaultWidget());
        if (!check) continue;

        // Ricollega la chiave tramite testo
        Etichette::Valore key;
        switch (counter) {
        case 0: key = Etichette::DizionarioComune; break;
        case 1: key = Etichette::Coniugazioni; break;
        case 2: key = Etichette::Approvate; break;
        case 3: key = Etichette::BonusRaro; break;
        case 4: key = Etichette::BonusStraniero; break;
        case 5: key = Etichette::BonusNome; break;
        default: continue;
        }

        // Blocca i segnali temporaneamente per evitare chiamate a toggle
        check->blockSignals(true);
        check->setChecked(et.haUnaEtichetta(Etichette(key)));
        check->blockSignals(false);

        ++counter;
    }
}


void CustomMenuButton::onMenuClosed() {
    if (!(etichette == etichette_originale)) {
        emit parolaModificata(text().toStdString(), etichette);

        aggiornaColoreSfondo();
    }
}



void CustomMenuButton::aggiornaColoreSfondo() {
    if (etichette.haUnaEtichetta(Etichette(Etichette::Approvate))) {
        this->setStyleSheet("background-color: #4CAF50; color: white;"); // Verde scuro, testo bianco
    } else if (etichette.haUnaEtichetta(Etichette(
                   Etichette::BonusNome |
                   Etichette::BonusRaro |
                   Etichette::BonusStraniero))) {
        this->setStyleSheet("background-color: #FFD700; color: black;"); // Giallo oro, testo nero
    } else {
        this->setStyleSheet(""); // Default
    }
}
