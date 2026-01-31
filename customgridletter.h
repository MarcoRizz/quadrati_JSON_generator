#ifndef CUSTOMGRIDLETTER_H
#define CUSTOMGRIDLETTER_H

#include <QLabel>
#include "custommenubutton.h"

class CustomGridLetter : public QLabel
{
    Q_OBJECT

public:
    explicit CustomGridLetter(QWidget* parent = nullptr);

    void connectWord(CustomMenuButton* word);
    void disconnectWord(CustomMenuButton* word);

    bool isUsed() const;
    bool isUsedBy(CustomMenuButton* word) const;

signals:
    void wordConnected(CustomMenuButton* word);
    void wordDisconnected(CustomMenuButton* word);

private slots:
    void onWordDestroyed(QObject* obj);

private:
    QSet<CustomMenuButton*> m_words;
};

#endif // CUSTOMGRIDLETTER_H
