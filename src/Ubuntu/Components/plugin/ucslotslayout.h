#ifndef UCSLOTSLAYOUT_H
#define UCSLOTSLAYOUT_H

#include <QtQuick/QQuickItem>
#include <QtGui/QPixmap>
#include <private/qquicktext_p.h>

class UCSlotsAttached;
class UCSlotsLayoutPrivate;

class UCSlotsLayout : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QQuickText *titleItem READ titleItem CONSTANT)
    Q_PROPERTY(QQuickText *subtitleItem READ subtitleItem CONSTANT)
    Q_PROPERTY(QQuickText *subsubtitleItem READ subsubtitleItem CONSTANT)
    Q_PROPERTY(qreal leftOffset READ leftOffset WRITE setLeftOffsetQml NOTIFY leftOffsetChanged)
    Q_PROPERTY(qreal rightOffset READ rightOffset WRITE setRightOffsetQml NOTIFY rightOffsetChanged)
    Q_PROPERTY(qreal topOffset READ topOffset WRITE setTopOffsetQml NOTIFY topOffsetChanged)
    Q_PROPERTY(qreal bottomOffset READ bottomOffset WRITE setBottomOffsetQml NOTIFY bottomOffsetChanged)
    Q_PROPERTY(bool progression READ progression WRITE setProgression NOTIFY progressionChanged)
    Q_PROPERTY(UCSlotsAttached *progressionSlot READ progressionSlot CONSTANT)

    Q_ENUMS(UCSlotPosition)

public:
    explicit UCSlotsLayout(QQuickItem *parent = 0);

    QQuickText *titleItem() const;
    QQuickText *subtitleItem() const;
    QQuickText *subsubtitleItem() const;

    qreal leftOffset() const;
    void setLeftOffset(qreal val);
    void setLeftOffsetQml(qreal val);
    bool leftOffsetWasSetFromQml;

    qreal rightOffset() const;
    void setRightOffset(qreal val);
    void setRightOffsetQml(qreal val);

    qreal topOffset() const;
    void setTopOffset(qreal val);
    void setTopOffsetQml(qreal val);

    qreal bottomOffset() const;
    void setBottomOffset(qreal val);
    void setBottomOffsetQml(qreal val);

    bool progression() const;
    void setProgression(bool val);

    UCSlotsAttached *progressionSlot() const;

    enum UCSlotPosition { Leading, Trailing };

    static UCSlotsAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    //TODO: should this be in the pimpl?
    void relayoutNeeded();

    void slotClicked(QQuickItem *slot);
    void leftOffsetChanged();
    void rightOffsetChanged();
    void topOffsetChanged();
    void bottomOffsetChanged();
    void progressionChanged();

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
    Q_PRIVATE_SLOT(d_func(), void _q_updateProgressionStatus())

};

class UCSlotsAttached : public QObject
{
    Q_OBJECT
    Q_PROPERTY(UCSlotsLayout::UCSlotPosition position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMarginQml NOTIFY leftMarginChanged)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMarginQml NOTIFY rightMarginChanged)
    Q_PROPERTY(bool overrideVerticalPositioning READ overrideVerticalPositioning WRITE setOverrideVerticalPositioning NOTIFY overrideVerticalPositioningChanged)

public:
    UCSlotsAttached(QObject *object);

    UCSlotsLayout::UCSlotPosition position() const;
    void setPosition(UCSlotsLayout::UCSlotPosition pos);

    qreal leftMargin() const;
    void setLeftMargin(qreal margin);
    void setLeftMarginQml(qreal margin);

    qreal rightMargin() const;
    void setRightMargin(qreal margin);
    void setRightMarginQml(qreal margin);

    bool overrideVerticalPositioning() const;
    void setOverrideVerticalPositioning(bool val);

public Q_SLOTS:
    void updateGuValues();

Q_SIGNALS:
    void positionChanged();
    void leftMarginChanged();
    void rightMarginChanged();
    void overrideVerticalPositioningChanged();

private:
    UCSlotsLayout::UCSlotPosition m_position;
    qreal m_leftMargin;
    qreal m_rightMargin;
    bool m_overrideVerticalPositioning;

    bool leftMarginWasSetFromQml;
    bool rightMarginWasSetFromQml;
};

class UCSlotsLayoutChevronPrivate;
class UCSlotsLayoutChevron : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit UCSlotsLayoutChevron(QQuickItem *parent = 0);
    void paint(QPainter *painter);

public Q_SLOTS:
    void updateGuValues();

protected:
    Q_DECLARE_PRIVATE(UCSlotsLayoutChevron)

private:
    void reloadIcon();
    QPixmap progressionPixmap;

};

QML_DECLARE_TYPEINFO(UCSlotsLayout, QML_HAS_ATTACHED_PROPERTIES)

#endif // UCSLOTSLAYOUT_H
