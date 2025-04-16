#ifndef CUSTOMMENUBUTTON_H
#define CUSTOMMENUBUTTON_H

#include <QPushButton>
#include <QMenu>
#include "etichette.h"

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
    explicit CustomMenuButton(const QString& text, const Etichette &et = Etichette(), QWidget* parent = nullptr);

private:
    Etichette etichette;
    PersistentMenu* menu;
};

#endif // CUSTOMMENUBUTTON_H
