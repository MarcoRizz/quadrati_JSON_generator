#include "widget_displayDictionary.h"
#include "Dizionario.h"  // includi la tua classe dizionario se serve

#include <QDebug>

widget_displayDictionary::widget_displayDictionary(QWidget *parent)
    : QWidget(parent)
{
    setMouseTracking(true);
}

void widget_displayDictionary::setLayoutAndButtons(QVBoxLayout *layout, const QVector<CustomMenuButton *> &btns) {
    layoutRef = layout;
    buttons = btns;

    if (layoutRef) {
        layoutRef->installEventFilter(this);
    }

    if (!buttons.isEmpty()) {
        parolaCentrale = buttons[buttons.size() / 2]->text();  // parola centrale iniziale
    }
}

void widget_displayDictionary::setDizionario(Dizionario *diz) {
    dizionario = diz;
}

bool widget_displayDictionary::displayParola(std::string parola) {

    if (!dizionario) return false;

    std::vector<std::string> lista = dizionario->cercaParoleInRange(parola, 10, 10); //TODO: qui bisogna recuperare anche tutte le etichette
    //TODO: verificare anche che questi pulsanti sovrascrivano correttamente il dizionario se viene modificata qualche parola.

    if (lista.size() == 20) return false;

    int count = std::min<int>(lista.size(), buttons.size());  // evita fuoriuscite

    for (int i = 0; i < count; ++i) {
        buttons[i]->setText(QString::fromStdString(lista[i]));
    }

    return false;
}

bool widget_displayDictionary::eventFilter(QObject *obj, QEvent *event) {
    if (obj == layoutRef && event->type() == QEvent::Wheel) {
        QWheelEvent *wheelEvent = static_cast<QWheelEvent *>(event);

        const QPoint cursorPos = mapFromGlobal(QCursor::pos());
        if (rect().contains(cursorPos)) {
            bool avanti = wheelEvent->angleDelta().y() < 0;
            scorriDizionario(avanti);
            return true;
        }
    }
    return QWidget::eventFilter(obj, event);
}

void widget_displayDictionary::scorriDizionario(bool avanti) {
    if (!dizionario || buttons.isEmpty()) return;

    QVector<QString> paroleCorrenti;
    for (QPushButton *btn : buttons) {
        paroleCorrenti.append(btn->text());
    }

    QVector<QString> nuoveParole;

    if (avanti) {
        // Scorri in avanti: calcola la nuova parola dopo l'ultima
        QString nuovaParola = QString::fromStdString(dizionario->cercaParolaSuccessiva(paroleCorrenti.last().toStdString()));

        // Elimina la prima parola e aggiungi in fondo quella nuova
        nuoveParole = paroleCorrenti.mid(1);
        nuoveParole.append(nuovaParola);
    } else {
        // Scorri all'indietro: calcola la nuova parola prima della prima
        QString nuovaParola = QString::fromStdString(dizionario->cercaParolaPrecedente(paroleCorrenti.first().toStdString()));

        // Aggiungi in cima e rimuovi l'ultima
        nuoveParole.append(nuovaParola);
        nuoveParole += paroleCorrenti.mid(0, paroleCorrenti.size() - 1);
    }

    // Aggiorna i bottoni
    for (int i = 0; i < buttons.size(); ++i) {
        buttons[i]->setText(nuoveParole[i]);
    }

    parolaCentrale = nuoveParole[buttons.size() / 2];
}


