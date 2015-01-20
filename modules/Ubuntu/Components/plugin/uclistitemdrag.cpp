
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>
#include <QtGui/QGuiApplication>
#include <QtQuick/private/qquickflickable_p.h>

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
            this, SLOT(setupDragMode(bool)));
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
