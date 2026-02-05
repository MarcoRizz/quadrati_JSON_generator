#include "CustomMenuButton.h"
#include "customgridletter.h"

#include <QWidgetAction>
#include <QPushButton>
#include <QDebug>
#include <QEvent>
#include <QStringList>
#include <QCheckBox>
#include <QMouseEvent>
#include <qboxlayout.h>

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
    : CustomMenuButton("CustomButton", Etichette(), QVector<QVector<CustomGridLetter*>> (), parent)
{}

// Costruttore della CustomMenuButton //TODO: impostare mainWindow->highlightTiles(path, parola.length()) al clic sul bottone
CustomMenuButton::CustomMenuButton(const QString& text, const Etichette &et, const QVector<QVector<CustomGridLetter *>> &percorsi, QWidget* parent)
    : QPushButton(text, parent), etichette(et), percorsi(percorsi)
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
        emit highLightW(this->text().toStdString());
        menu->exec(this->mapToGlobal(QPoint(0, this->height())));
    });

    // Assegno il colore
    aggiornaColoreSfondo();
}

void CustomMenuButton::cambiaParola(const QString& text, const Etichette &et, const QVector<QVector<CustomGridLetter *> > &perc) {
    setText(text);
    etichette = et;
    etichette_originale = et;
    percorsi = perc;

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


bool CustomMenuButton::isBonus() const
{
    QWidget* p = parentWidget();
    while (p) {
        if (p->objectName() == "boxBonus") {
            return true;
        }
        p = p->parentWidget();
    }
    return false;
}


int CustomMenuButton::getAlphabeticalIndex() const
{
    QWidget* p = parentWidget();
    if (!p) return -1;

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(p->layout());
    if (!layout) return -1;

    QString myText = text();
    int index = 0;

    for (int i = 0; i < layout->count(); ++i) {
        QWidget* w = layout->itemAt(i)->widget();
        if (!w || w == this) break; // siamo arrivati a noi
        if (auto* btn = qobject_cast<CustomMenuButton*>(w)) {
            if (QString::compare(btn->text(), myText, Qt::CaseInsensitive) < 0) {
                ++index;
            } else {
                break;
            }
        }
    }

    return index;
}

bool CustomMenuButton::addPercorso(const QVector<CustomGridLetter*>& percorso)
{
    qDebug() << "entro in addPercorso";
    if (percorso.isEmpty() || percorsi.contains(percorso))
        return false;

    for (auto element : percorso) {
        qDebug() << element->text();
    }
    percorsi.append(percorso);

    for (auto tile : percorso)
        connect(tile, &CustomGridLetter::tileChanged,
                this, &CustomMenuButton::checkPaths);

    return true;
}


bool CustomMenuButton::removePercorso(const QVector<CustomGridLetter*>& percorso)
{
    int before = percorsi.size();
    percorsi.removeAll(percorso);
    return percorsi.size() < before;
}


bool CustomMenuButton::removePercorso(int index)
{
    if (index < 0 || index >= percorsi.size())
        return false;

    percorsi.removeAt(index);
    return true;
}


void CustomMenuButton::clearPercorsi()
{
    percorsi.clear();
}


const QVector<QVector<CustomGridLetter*>>& CustomMenuButton::getPercorsi() const
{
    return percorsi;
}


void CustomMenuButton::checkPaths(CustomGridLetter* tile_eliminata)
{
    // Rimuove tutti i percorsi che contengono tile_eliminata
    for (int i = percorsi.size() - 1; i >= 0; --i) {
        if (percorsi[i].contains(tile_eliminata)) {
            percorsi.removeAt(i);
        }
    }

    // Se non rimane alcun percorso, emetti un segnale per far eliminare il bottone
    if (percorsi.isEmpty()) {
        emit toEliminate(this);
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
