#ifndef CUSTOMMENUBUTTON_H
#define CUSTOMMENUBUTTON_H

#include <QPushButton>
#include <QMenu>
#include "etichette.h"
#include "generate_JSON.h"

class PersistentMenu : public QMenu {
    Q_OBJECT

public:
    explicit PersistentMenu(QWidget* parent = nullptr);

protected:
    bool event(QEvent* e) override;
};

class CustomMenuButton : public QPushButton {
    Q_OBJECT

public:
    explicit CustomMenuButton(const QString& text, const Etichette &et, Generate_JSON* generate_json, QWidget* parent = nullptr);

private:
    Etichette etichette;
    Etichette etichette_originale;
    PersistentMenu* menu;
    Generate_JSON* gen_JSON_address;
    void aggiornaColoreSfondo();

private slots:
    void onMenuClosed();

};

#endif // CUSTOMMENUBUTTON_H
