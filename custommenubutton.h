#ifndef CUSTOMMENUBUTTON_H
#define CUSTOMMENUBUTTON_H

#include <QPushButton>
#include <QMenu>
#include <qlabel.h>
#include "etichette.h"

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
        const QVector<QVector<QLabel*>>& percorsi = {},
        QWidget* parent = nullptr);

    void cambiaParola(const QString& text, const Etichette &et = Etichette(), const QVector<QVector<QLabel*>>& percorsi = {});

    void addPercorso(const QVector<QLabel*>& percorso);
    void removePercorso(int index);
    void clearPercorsi();

    const QVector<QVector<QLabel*>>& getPercorsi() const;

signals:
    void parolaModificata(const std::string& testo, const Etichette& etichette);
    void highLightW(const std::string& testo);


private:
    Etichette etichette;
    Etichette etichette_originale;
    QVector<QVector<QLabel*>> percorsi;

    PersistentMenu* menu;

    void aggiornaColoreSfondo();

private slots:
    void onMenuClosed();

};

#endif // CUSTOMMENUBUTTON_H
