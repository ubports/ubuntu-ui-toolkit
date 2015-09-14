#ifndef UCSLOTSLAYOUT_H
#define UCSLOTSLAYOUT_H

#include <QtQuick/QQuickItem>
#include <QtGui/QPixmap>
#include <private/qquicktext_p.h>

class UCSlotsAttached;
class UCSlotsLayoutMargins;
class UCSlotsLayoutPrivate;
class UCSlotsLayout : public QQuickItem
{
    Q_OBJECT

    Q_PROPERTY(QQuickText *titleItem READ titleItem CONSTANT FINAL)
    Q_PROPERTY(QQuickText *subtitleItem READ subtitleItem CONSTANT FINAL)
    Q_PROPERTY(QQuickText *subsubtitleItem READ subsubtitleItem CONSTANT FINAL)
    Q_PROPERTY(UCSlotsLayoutMargins *contentMargins READ contentMargins CONSTANT FINAL)
    Q_PROPERTY(UCSlotsAttached *progressionSlot READ progressionSlot CONSTANT FINAL)
    Q_PROPERTY(bool progression READ progression WRITE setProgression NOTIFY progressionChanged FINAL)

    Q_ENUMS(UCSlotPosition)

public:
    explicit UCSlotsLayout(QQuickItem *parent = 0);

    //this methods can't be const because otherwise they'd have to return
    //unmodifiable labels, since they're allocated on the stack, and that would
    //fail compilation (unless const_cast is used)
    QQuickText *titleItem();
    QQuickText *subtitleItem();
    QQuickText *subsubtitleItem();

    UCSlotsLayoutMargins *contentMargins();

    bool progression() const;
    void setProgression(bool val);

    UCSlotsAttached *progressionSlot() const;

    enum UCSlotPosition { Leading, Trailing };

    static UCSlotsAttached *qmlAttachedProperties(QObject *object);

Q_SIGNALS:
    //TODO: should this be in the pimpl?
    void relayoutNeeded();
    void progressionChanged();

protected:
    Q_DECLARE_PRIVATE(UCSlotsLayout)
    void componentComplete();
    void itemChange(ItemChange change, const ItemChangeData &data);

private:
    Q_PRIVATE_SLOT(d_func(), void _q_onThemeChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_onGuValueChanged())
    Q_PRIVATE_SLOT(d_func(), void _q_updateCachedHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateProgressionStatus())
    Q_PRIVATE_SLOT(d_func(), void _q_updateGuValues())
    Q_PRIVATE_SLOT(d_func(), void _q_updateLabelsAnchorsAndBBoxHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSlotsBBoxHeight())
    Q_PRIVATE_SLOT(d_func(), void _q_updateSize())
    Q_PRIVATE_SLOT(d_func(), void _q_relayout())
};
QML_DECLARE_TYPEINFO(UCSlotsLayout, QML_HAS_ATTACHED_PROPERTIES)


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
    bool m_overrideVerticalPositioning : 1;

    bool leftMarginWasSetFromQml : 1;
    bool rightMarginWasSetFromQml : 1;
};

class UCSlotsLayoutChevron : public QQuickPaintedItem
{
    Q_OBJECT

public:
    explicit UCSlotsLayoutChevron(QQuickItem *parent = 0);
    void paint(QPainter *painter);

public Q_SLOTS:
    void updateGuValues();

private:
    void reloadIcon();
    QPixmap progressionPixmap;

};

class UCSlotsLayoutMargins : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal leftMargin READ leftMargin WRITE setLeftMarginQml NOTIFY leftMarginChanged FINAL)
    Q_PROPERTY(qreal rightMargin READ rightMargin WRITE setRightMarginQml NOTIFY rightMarginChanged FINAL)
    Q_PROPERTY(qreal topMargin READ topMargin WRITE setTopMarginQml NOTIFY topMarginChanged FINAL)
    Q_PROPERTY(qreal bottomMargin READ bottomMargin WRITE setBottomMarginQml NOTIFY bottomMarginChanged FINAL)

public:
    explicit UCSlotsLayoutMargins(QObject *parent = 0);

    qreal leftMargin() const;
    void setLeftMargin(qreal val);
    void setLeftMarginQml(qreal val);

    qreal rightMargin() const;
    void setRightMargin(qreal val);
    void setRightMarginQml(qreal val);

    qreal topMargin() const;
    void setTopMargin(qreal val);
    void setTopMarginQml(qreal val);

    qreal bottomMargin() const;
    void setBottomMargin(qreal val);
    void setBottomMarginQml(qreal val);

    //once the dev tries to change the offsets (and he does so via QML) we'll stop
    //updating offset's value, for instance when gu value changes or when the
    //positioning mode changes
    bool leftMarginWasSetFromQml : 1;
    bool rightMarginWasSetFromQml : 1;
    bool topMarginWasSetFromQml : 1;
    bool bottomMarginWasSetFromQml : 1;

Q_SIGNALS:
    void leftMarginChanged();
    void rightMarginChanged();
    void topMarginChanged();
    void bottomMarginChanged();

private:
    //similar to anchors.margins, but we don't use a contentItem so we handle this ourselves
    qreal m_leftMargin;
    qreal m_rightMargin;
    qreal m_topMargin;
    qreal m_bottomMargin;
};

#endif // UCSLOTSLAYOUT_H
