#ifndef UCSLOTSLAYOUTPRIVATE_H
#define UCSLOTSLAYOUTPRIVATE_H

#include <QtQuick/private/qquickitem_p.h>
#include "ucslotslayout.h"

#define IMPLICIT_SLOTSLAYOUT_WIDTH_GU      40
#define IMPLICIT_SLOTSLAYOUT_HEIGHT_GU     7
#define IMPLICIT_SLOTSLAYOUT_MARGIN        2
#define SLOTSLAYOUT_LABELS_SPACING         1
#define SLOTSLAYOUT_LABELS_RIGHTMARGIN     2

class UCSlotsLayoutPrivate : QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(UCSlotsLayout)
public:
    UCSlotsLayoutPrivate();
    virtual ~UCSlotsLayoutPrivate();
    void init();

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

    bool ready;

    //used for position handling
    qreal labelsBoundingBoxHeight;
    //maximum height of the components (no labels) which are inside the slots, used to compute the
    //height of the layout
    qreal maxChildrenRectHeight;

    //this is used so that we only call relayout if these values go from 0 to non-0,
    //not every time
    qreal _q_cachedHeight;

    QQuickText m_title;
    QQuickText m_subtitle;
    QQuickText m_subsubtitle;

    //in this variable we cache the current parent so that we can disconnect from the signals
    //when the parent changes. We need this because otherwise inside itemChange(..) we would
    //only have access to the new parent
    QQuickItem* m_parentItem;

private:
    void setDefaultLabelsProperties();

};

#endif // UCSLOTSLAYOUTPRIVATE_H
