#ifndef UCSCROLLBAR_P_H
#define UCSCROLLBAR_P_H

#include <ucstyleditembase_p.h>
#include <ucscrollbar.h>

class QQuickFlickable;
class UCScrollbarPrivate : public UCStyledItemBasePrivate
{
    Q_DECLARE_PUBLIC(UCScrollbar)

public:
    UCScrollbarPrivate();
    ~UCScrollbarPrivate();

    static const UCScrollbarPrivate *get(const UCScrollbar *item) { return item->d_func(); }
    static UCScrollbarPrivate *get(UCScrollbar *item) { return item->d_func(); }

    void init();

    QQuickFlickable *flickableItem;
    UCScrollbar::UCScrollbarAlignment alignment;
    UCScrollbar *buddyScrollbar;
    QQuickItem *viewport;
    bool alwaysOnScrollbar;
    bool interactive;
};

#endif // UCSCROLLBAR_P_H

