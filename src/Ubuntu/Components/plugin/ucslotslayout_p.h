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
    //This method is called whenever we want to relayout the items
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

    //connect item's UCSlotsAttached properties changes to the right slot
    void connectAttachedPropertySignals(QQuickItem *item);

    bool ready;
    QQuickItem *pressedItem;

    //used for position handling
    qreal labelsBoundingBoxHeight;
    //maximum height of the components (no labels) which are inside the slots, used to compute the
    //height of the layout
    qreal maxChildrenHeight;

    //this is used so that we only call relayout if these values go from 0 to non-0,
    //not every time
    qreal _q_cachedHeight;

    QQuickText m_title;
    QQuickText m_subtitle;
    QQuickText m_subsubtitle;

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
    //updating offset's value, for instance when gu value changes or when the visual
    //rules would require a different offset than usual
    bool leftOffsetWasSetFromQml;
    bool rightOffsetWasSetFromQml;
    bool topOffsetWasSetFromQml;
    bool bottomOffsetWasSetFromQml;

    //in this variable we cache the current parent so that we can disconnect from the signals
    //when the parent changes. We need this because otherwise inside itemChange(..) we would
    //only have access to the new parent
    QQuickItem *m_parentItem;

private:
    void setDefaultLabelsProperties();

};

#endif // UCSLOTSLAYOUTPRIVATE_H
