#ifndef UCSLOTSLAYOUTPRIVATE_H
#define UCSLOTSLAYOUTPRIVATE_H

#include <private/qquickitem_p.h>

#include "ucslotslayout.h"

#define IMPLICIT_SLOTSLAYOUT_WIDTH_GU                40
#define IMPLICIT_SLOTSLAYOUT_HEIGHT_GU               7
#define SLOTSLAYOUT_LABELS_SPACING_GU                1
#define SLOTSLAYOUT_LABELS_SIDEMARGINS_GU            1
#define SLOTSLAYOUT_SLOTS_SIDEMARGINS_GU             1
#define SLOTSLAYOUT_LEFTOFFSET_GU                    1
#define SLOTSLAYOUT_RIGHTOFFSET_GU                   1

//we want a different top/bottom margin if any of the slot is taller than this
#define SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD_GU 4
#define SLOTSLAYOUT_TOPMARGIN1_GU                    1
#define SLOTSLAYOUT_TOPMARGIN2_GU                    2
#define SLOTSLAYOUT_BOTTOMMARGIN1_GU                 1
#define SLOTSLAYOUT_BOTTOMMARGIN2_GU                 2

//TODO: Update this when the values are final
#define CHEVRON_DEFAULT_HEIGHT_GU                    2.5
#define CHEVRON_DEFAULT_WIDTH_GU                     2

class UCSlotsLayoutPrivate : QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(UCSlotsLayout)
public:
    UCSlotsLayoutPrivate();
    virtual ~UCSlotsLayoutPrivate();
    void init();
    void updateTopBottomOffsetsIfNeeded();

    //We have two vertical positioning modes according to the visual design rules:
    //- RETURN VALUE CenterVertically --> All items have to be vertically centered
    //- RETURN VALUE AlignToTop --> All items have to anchor to the top of the listitem (using a top margin as well)
    //This is mainly used by the layout method
    enum UCSlotPositioningMode { AlignToTop, CenterVertically };
    UCSlotsLayoutPrivate::UCSlotPositioningMode getVerticalPositioningMode() const;

    //connect/disconnect item's UCSlotsAttached properties changes to/from the right slot
    void handleAttachedPropertySignals(QQuickItem *item, bool connect);

    static inline UCSlotsLayoutPrivate *get(UCSlotsLayout *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    void _q_onThemeChanged();
    void _q_onGuValueChanged();
    void _q_updateCachedHeight();
    void _q_updateProgressionStatus();
    void _q_updateGuValues();
    void _q_updateLabelsAnchorsAndBBoxHeight();
    void _q_updateSlotsBBoxHeight();
    void _q_updateSize();
    void _q_relayout();

    QQuickText m_title;
    QQuickText m_subtitle;
    QQuickText m_subsubtitle;

    //We cache the current parent so that we can disconnect from the signals when the
    //parent changes. We need this because itemChange(..) only provides the new parent
    QQuickItem *m_parentItem;

    QQuickItem *pressedItem;
    UCSlotsLayoutChevron *chevron;

    qreal labelsBoundingBoxHeight;
    //max slots height ignoring labels
    qreal maxSlotsHeight;
    //we cache the height so that we only relayout when it goes
    //from 0 to non-0 and not viceversa
    qreal _q_cachedHeight;

    //similar to anchors.margins, but we don't use a contentItem so we handle this ourselves
    qreal leftOffset;
    qreal rightOffset;
    qreal topOffset;
    qreal bottomOffset;

    //currently fixed, but we may allow changing this in the future
    qint32 maxNumberOfLeadingSlots;
    qint32 maxNumberOfTrailingSlots;

    //once the dev tries to change the offsets (and he does so via QML) we'll stop
    //updating offset's value, for instance when gu value changes or when the
    //positioning mode changes
    bool leftOffsetWasSetFromQml : 1;
    bool rightOffsetWasSetFromQml : 1;
    bool topOffsetWasSetFromQml : 1;
    bool bottomOffsetWasSetFromQml : 1;

    //Show the chevron, name taken from old ListItem API to minimize changes
    bool progression : 1;
    bool ready : 1;

private:
    void setDefaultLabelsProperties();
};

#endif // UCSLOTSLAYOUTPRIVATE_H
