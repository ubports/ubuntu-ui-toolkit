#ifndef UCSCROLLBAR_H
#define UCSCROLLBAR_H

#include <ucstyleditembase.h>
#include <qnamespace.h>

class QQuickFlickable;
class UCScrollbarPrivate;
class UCScrollbar : public UCStyledItemBase
{
    Q_OBJECT

    Q_PROPERTY(QQuickFlickable *flickableItem READ flickableItem WRITE setFlickableItem NOTIFY flickableItemChanged)
    Q_PROPERTY(UCScrollbarAlignment align READ align WRITE setAlign NOTIFY alignChanged)
    Q_PROPERTY(UCScrollbar *__buddyScrollbar READ buddyScrollbar WRITE setBuddyScrollbar NOTIFY buddyScrollbarChanged)
    Q_PROPERTY(QQuickItem *__viewport READ viewport WRITE setViewport NOTIFY viewportChanged)
    //TODO: qml version was using alwaysOnScrollbarS, but it should be singular
    Q_PROPERTY(bool __alwaysOnScrollbars READ alwaysOnScrollbar WRITE setAlwaysOnScrollbar NOTIFY alwaysOnScrollbarChanged)
    Q_PROPERTY(bool __interactive READ interactive WRITE setInteractive NOTIFY interactiveChanged)

    Q_ENUMS(UCScrollbarAlignment)

public:
    enum UCScrollbarAlignment {
        AlignTop = Qt::AlignTop,
        AlignBottom = Qt::AlignBottom,
        AlignLeading = Qt::AlignLeading,
        AlignTrailing = Qt::AlignTrailing
    };

    UCScrollbar(QQuickItem *parent = 0);

    QQuickFlickable *flickableItem();
    void setFlickableItem(QQuickFlickable *item);

    UCScrollbarAlignment align();
    void setAlign(UCScrollbarAlignment val);

    UCScrollbar *buddyScrollbar();
    void setBuddyScrollbar(UCScrollbar *buddy);

    QQuickItem *viewport();
    void setViewport(QQuickItem *viewport);

    bool interactive();
    void setInteractive(bool val);

    bool alwaysOnScrollbar();
    void setAlwaysOnScrollbar(bool val);

Q_SIGNALS:
    void flickableItemChanged();
    void alignChanged();
    void buddyScrollbarChanged();
    void viewportChanged();
    void interactiveChanged();
    void alwaysOnScrollbarChanged();

private:
    Q_DISABLE_COPY(UCScrollbar)
    Q_DECLARE_PRIVATE(UCScrollbar)

};

#endif // UCSCROLLBAR_H
