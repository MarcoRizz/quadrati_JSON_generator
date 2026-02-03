#ifndef CUSTOMMENUBUTTON_H
#define CUSTOMMENUBUTTON_H

#include <QPushButton>
#include <QMenu>
#include <qlabel.h>
#include "etichette.h"

class CustomGridLetter;

class PersistentMenu : public QMenu {
    Q_OBJECT

public:
    explicit PersistentMenu(QWidget* parent = nullptr);

protected:
    bool event(QEvent* e) override;
};

//=============================================================================

class CustomMenuButton : public QPushButton {
    Q_OBJECT

public:
    explicit CustomMenuButton(QWidget* parent);

    explicit CustomMenuButton(
        const QString& text = "CustomButton",
        const Etichette &et = Etichette(),
        const QVector<QVector<CustomGridLetter*>>& percorsi = {},
        QWidget* parent = nullptr);

    void cambiaParola(const QString& text, const Etichette &et = Etichette(), const QVector<QVector<CustomGridLetter*>>& percorsi = {});
    bool isBonus() const;
    int getAlphabeticalIndex() const;

    bool addPercorso(const QVector<CustomGridLetter*>& percorso);
    bool removePercorso(const QVector<CustomGridLetter*>& percorso);
    bool removePercorso(int index);
    void checkPaths(CustomGridLetter* tile);
    void clearPercorsi();

    const QVector<QVector<CustomGridLetter*>>& getPercorsi() const;

signals:
    void parolaModificata(const std::string& testo, const Etichette& etichette);
    void highLightW(const std::string& testo);
    void toEliminate(CustomMenuButton* parola);


private:
    Etichette etichette;
    Etichette etichette_originale;
    QVector<QVector<CustomGridLetter*>> percorsi;

    PersistentMenu* menu;

    void aggiornaColoreSfondo();

private slots:
    void onMenuClosed();

};

#endif // CUSTOMMENUBUTTON_H
