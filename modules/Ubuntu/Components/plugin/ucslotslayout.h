#ifndef UCSLOTSLAYOUT_H
#define UCSLOTSLAYOUT_H

#include <QtQuick/QQuickItem>
#include "private/qquicktext_p.h"

class UCSlotsLayoutPrivate;

class UCSlotsLayout : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QQuickText* titleItem READ titleItem CONSTANT)
    Q_PROPERTY(QQuickText* subtitleItem READ subtitleItem CONSTANT)
    Q_PROPERTY(QQuickText* subsubtitleItem READ subsubtitleItem CONSTANT)

public:
    explicit UCSlotsLayout(QQuickItem *parent = 0);

    QQuickText* titleItem() const;
    QQuickText* subtitleItem() const;
    QQuickText* subsubtitleItem() const;

protected:
    Q_DECLARE_PRIVATE(UCSlotsLayout)
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);

private:
    //this is only needed for Qt4/non-C++11, the macro expands to an empty string!
    //this is a request that moc will process and expand accordingly in moc phase
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
    Q_PRIVATE_SLOT(d_func(), void _q_updateLabelsAnchors())
    Q_PRIVATE_SLOT(d_func(), void _q_relayout())
};

#endif // UCSLOTSLAYOUT_H
