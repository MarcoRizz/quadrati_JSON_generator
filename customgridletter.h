#ifndef CUSTOMGRIDLETTER_H
#define CUSTOMGRIDLETTER_H

#include <QLabel>
#include "custommenubutton.h"

class CustomGridLetter : public QLabel
{
    Q_OBJECT

public:
    explicit CustomGridLetter(QWidget* parent = nullptr);

    void setText(const QString& text);

    void connectWord(CustomMenuButton* word, bool isBonus = false);
    void disconnectWord(CustomMenuButton* word);

    bool isUsed() const;
    bool isUsedBy(CustomMenuButton* word) const;

    void setUnchanged(bool set);
    bool isUnchanged() const;

signals:
    void wordConnected(CustomMenuButton* word);
    void wordDisconnected(CustomMenuButton* word);

private slots:
    void onWordDestroyed(QObject* obj);

private:
    QSet<CustomMenuButton*> m_words;
    QSet<CustomMenuButton*> m_words_bonus;
    bool unchanged = false;
};

#endif // CUSTOMGRIDLETTER_H
