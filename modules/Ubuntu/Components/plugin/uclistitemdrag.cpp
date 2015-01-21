
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>
#include <QtGui/QGuiApplication>
#include <QtQuick/private/qquickflickable_p.h>

/*!
 * \qmltype ListItemDrag
 * \inqmlmodule Ubuntu.Components 1.2
 * \ingroup unstable-ubuntu-listitems
 * \since Ubuntu.Components 1.2
 * \brief Provides information about a ListItem drag event.
 *
 * The object cannot be instantiated and it is passed as parameter to \l ViewItems::draggingStarted
 * and \l ViewItems::draggingUpdated attached signals. Developer can decide whether
 * to accept or restrict the dragging event based on the input provided by this
 * event.
 *
 * The direction of the drag can be found via the \l direction property and the
 * source and destination the drag can be applied via \l from and \l to properties.
 * The allowed directions can be configured through \l minimumIndex and \l maximumIndex
 * properties, and the event acceptance through \l accept property. If the event is not
 * accepted, the drag action will be considered as cancelled.
 */

/*!
 * \qmlproperty enum ListItemDrag::direction
 * \readonly
 * The property specifies the direction of the drag. Its value depends on the
 * signal triggered and can be one of the following:
 * \list
 *  \li \c ListItemDrag.None - no drag, value set when \l ViewItems::draggingStarted
 *         signal is emitted or when the \l ViewItems::draggingUpdated signal
 *         identifies a drop gesture.
 *  \li \c ListItemDrag.Upwards - the drag is proceeded upwards in the ListView
 *  \li \c ListItemDrag.Downwards - the drag proceeds downwards in the ListView
 * \endlist
 */

/*!
 * \qmlproperty int ListItemDrag::from
 * \readonly
 */
/*!
 * \qmlproperty int ListItemDrag::to
 * \readonly
 *
 * Specifies the source index the ListItem is dragged from and the destination
 * index it can be dropped.
 */

/*!
 * \qmlproperty int ListItemDrag::minimumIndex
 */
/*!
 * \qmlproperty int ListItemDrag::maximumIndex
 * These properties configure the minimum and maximum indexes the item can be
 * dragged. The properties can be set in \l ViewItems::draggingStarted signal,
 * changing them in \l ViewItems::draggingUpdated will have no effect. A value
 * of -1 means no restriction defined on the dragging interval side.
 */

/*!
 * \qmlproperty bool ListItemDrag::accept
 * The property can be used to dismiss the event. By default its value is true,
 * meaning the drag event is accepted.
 */


UCDragHandler::UCDragHandler(UCListItem *listItem)
    : UCHandlerBase(listItem)
    , dragging(false)
{
}

// listen for attached property's draggable change signal to activate dragging mode on the list item
void UCDragHandler::initialize(bool animated)
{
    UCListItemPrivate *pListItem = UCListItemPrivate::get(listItem);
    if (!pListItem->parentAttached) {
        return;
    }
    connect(pListItem->parentAttached, SIGNAL(dragModeChanged()),
            this, SLOT(setupDragMode()));
    if (pListItem->isDraggable()) {
        setupDragMode(animated);
    }
}

void UCDragHandler::setupDragMode(bool animated)
{
    UCListItemPrivate *pListItem = UCListItemPrivate::get(listItem);
    // make sure the ListItem is snapped out
    bool draggable = pListItem->isDraggable();
    if (draggable) {
        pListItem->promptRebound();
        // animate panel only in case is called due to a signal emit
        pListItem->initStyleItem();
        if (!panel && pListItem->styleItem && pListItem->styleItem->m_dragHandlerDelegate) {
            bool animate = animated || (senderSignalIndex() >= 0);
            setupPanel(pListItem->styleItem->m_dragHandlerDelegate, animate);
        }
    }

    // update visuals
    pListItem->update();
}
