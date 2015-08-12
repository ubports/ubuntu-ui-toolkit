#ifndef UCSLOTSLAYOUT_H
#define UCSLOTSLAYOUT_H

#include <QtQuick/QQuickItem>
#include "private/qquicktext_p.h"

class UCSlotsAttached;
class UCSlotsLayoutPrivate;

class UCSlotsLayout : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QQuickText* titleItem READ titleItem CONSTANT)
    Q_PROPERTY(QQuickText* subtitleItem READ subtitleItem CONSTANT)
    Q_PROPERTY(QQuickText* subsubtitleItem READ subsubtitleItem CONSTANT)
    Q_ENUMS(UCSlotPosition)

public:
    explicit UCSlotsLayout(QQuickItem *parent = 0);

    QQuickText* titleItem() const;
    QQuickText* subtitleItem() const;
    QQuickText* subsubtitleItem() const;

    enum UCSlotPosition { Leading, Trailing };

    static UCSlotsAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    //TODO: should this be in the pimpl?
    void relayoutNeeded();

    void slotClicked(QQuickItem* slot);

protected:
    Q_DECLARE_PRIVATE(UCSlotsLayout)
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private:
    //this is only needed for Qt4/non-C++11, the macro expands to an empty string!
    //this is a request that moc will process and expand accordingly in moc phase
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
    Q_PRIVATE_SLOT(d_func(), void _q_updateLabelsAnchorsAndBBoxHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_relayout())
    Q_PRIVATE_SLOT(d_func(), void _q_updateCachedHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSlotsBBoxHeight())


};

class UCSlotsAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UCSlotsLayout::UCSlotPosition position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMargin NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMargin NOTIFY rightMarginChanged)

public:
    UCSlotsAttached(QObject *object);

    UCSlotsLayout::UCSlotPosition position() const;
    void setPosition(UCSlotsLayout::UCSlotPosition pos);

    qreal leftMargin() const;
    void setLeftMargin(qreal margin);

    qreal rightMargin() const;
    void setRightMargin(qreal margin);

public Q_SLOTS:
    void updateGuValues();

Q_SIGNALS:
    void positionChanged();
    void leftMarginChanged();
    void rightMarginChanged();

private:
    UCSlotsLayout::UCSlotPosition m_position;
    qreal m_leftMargin;
    qreal m_rightMargin;
};

QML_DECLARE_TYPEINFO(UCSlotsLayout, QML_HAS_ATTACHED_PROPERTIES)

#endif // UCSLOTSLAYOUT_H
