#include "customgridletter.h"
#include "custommenubutton.h"

CustomGridLetter::CustomGridLetter(QWidget* parent)
    : QLabel(parent)
{
}

void CustomGridLetter::setText(const QString& text)
{
    QLabel::setText(text);  // comportamento originale
    unchanged = false;      // tua logica aggiuntiva
    emit tileChanged(this);
}

void CustomGridLetter::connectWord(CustomMenuButton* word, bool isBonus)
{
    if (!word || m_words.contains(word) || m_words_bonus.contains(word))
        return;

    if (isBonus) {
        m_words_bonus.insert(word);
    } else {
        m_words.insert(word);
    }

    // Se il bottone viene distrutto, lo rimuoviamo automaticamente
    connect(word, &QObject::destroyed,
            this, &CustomGridLetter::onWordDestroyed);

    emit wordConnected(word);
}

void CustomGridLetter::disconnectWord(CustomMenuButton* word)
{
    if (m_words.contains(word)) {
        m_words.remove(word);
        emit wordDisconnected(word);
    }
    if (m_words_bonus.contains(word)) {
        m_words_bonus.remove(word);
        emit wordDisconnected(word);
    }
    return;
}

void CustomGridLetter::onWordDestroyed(QObject* obj)
{
    auto* word = static_cast<CustomMenuButton*>(obj);

    if (m_words.remove(word))
        emit wordDisconnected(word);
    if (m_words_bonus.remove(word))
        emit wordDisconnected(word);
}

bool CustomGridLetter::isUsed() const
{
    return !m_words.isEmpty();
}

bool CustomGridLetter::isUsedBy(CustomMenuButton* word) const
{
    return m_words.contains(word) || m_words_bonus.contains(word);
}

QSet<CustomMenuButton*> CustomGridLetter::UsedBy(bool fromBonus) const
{
    if (fromBonus)
        return m_words_bonus;
    else
        return m_words;
}

void CustomGridLetter::setUnchanged(bool set)
{
    unchanged = set;
    return;
}

bool CustomGridLetter::isUnchanged() const
{
    return unchanged;
}
