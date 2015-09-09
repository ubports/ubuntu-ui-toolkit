#ifndef UCSLOTSLAYOUTPRIVATE_H
#define UCSLOTSLAYOUTPRIVATE_H

#include <private/qquickitem_p.h>

#include "ucslotslayout.h"

#define IMPLICIT_SLOTSLAYOUT_WIDTH_GU             40
#define IMPLICIT_SLOTSLAYOUT_HEIGHT_GU            7
#define SLOTSLAYOUT_LABELS_SPACING                1
#define SLOTSLAYOUT_LABELS_SIDEMARGINS            1
#define SLOTSLAYOUT_DEFAULTSLOTSIDEMARGINS_GU     1
#define SLOTSLAYOUT_DEFAULTLAYOUTSIDEMARGINS_GU   1
//we want a different top/bottom margin if any of the slot is taller than this
#define SLOTSLAYOUT_TOPBOTTOMMARGIN_SIZETHRESHOLD 4

//TODO: Update this when the values are final
#define CHEVRON_DEFAULT_HEIGHT_GU                 2.5
#define CHEVRON_DEFAULT_WIDTH_GU                  2

class UCSlotsLayoutPrivate : QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(UCSlotsLayout)
public:
    UCSlotsLayoutPrivate();
    virtual ~UCSlotsLayoutPrivate();
    void init();
    void updateTopBottomOffsetsIfNeeded();

    //We have two vertical positioning modes according to the visual design rules:
    //- RETURN VALUE 0 --> All items have to be vertically centered
    //- RETURN VALUE 1 --> All items have to anchor to the top of the listitem (using a top margin as well)
    //This is mainly used by the layout method
    int getVerticalPositioningMode();

    static inline UCSlotsLayoutPrivate *get(UCSlotsLayout *that)
    {
        Q_ASSERT(that);
        return that->d_func();
    }

    void _q_relayout();
    void _q_updateSize();
    void _q_updateSlotsBBoxHeight();
    void _q_updateLabelsAnchorsAndBBoxHeight();
    void _q_updateCachedHeight();
    void _q_updateProgressionStatus();

    //connect/disconnect item's UCSlotsAttached properties changes to/from the right slot
    void handleAttachedPropertySignals(QQuickItem *item, bool connect);

    bool ready;
    QQuickItem *pressedItem;

    qreal labelsBoundingBoxHeight;
    //max slots height ignoring labels
    qreal maxSlotsHeight;

    //we cache the height so that we only relayout when it goes
    //from 0 to non-0 and not viceversa
    qreal _q_cachedHeight;

    QQuickText m_title;
    QQuickText m_subtitle;
    QQuickText m_subsubtitle;

    //similar to anchors.margins, but we don't use a contentItem so we handle this ourselves
    qreal leftOffset;
    qreal rightOffset;
    qreal topOffset;
    qreal bottomOffset;

    //Show the chevron, name taken from old ListItem API to minimize changes
    bool progression;
    UCSlotsLayoutChevron *chevron;

    //currently fixed, but we may allow changing this in the future
    qint32 maxNumberOfLeadingSlots;
    qint32 maxNumberOfTrailingSlots;

    //once the dev tries to change the offsets (and he does so via QML) we'll stop
    //updating offset's value, for instance when gu value changes or when the
    //positioning mode changes
    bool leftOffsetWasSetFromQml;
    bool rightOffsetWasSetFromQml;
    bool topOffsetWasSetFromQml;
    bool bottomOffsetWasSetFromQml;

    //We cache the current parent so that we can disconnect from the signals when the
    //parent changes. We need this because itemChange(..) only provides the new parent
    QQuickItem *m_parentItem;

private:
    void setDefaultLabelsProperties();

};

#endif // UCSLOTSLAYOUTPRIVATE_H
