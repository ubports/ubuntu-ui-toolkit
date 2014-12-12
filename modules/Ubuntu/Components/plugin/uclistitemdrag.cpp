
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
    connect(this, &UCDragHandler::draggingChanged,
            listItem, &UCListItem::draggingChanged);
}

// listen for attached property's draggable change signal to activate dragging mode on the list item
void UCDragHandler::initialize()
{
    if (!listItem->attachedProperties) {
        return;
    }
    connect(listItem->attachedProperties, &UCListItemAttached::dragModeChanged,
            this, &UCDragHandler::setupDragMode);
    if (listItem->isDraggable()) {
        setupDragMode();
    }
}

void UCDragHandler::setupDragMode()
{
    // make sure the ListItem is snapped out
    bool draggable = listItem->isDraggable();
    if (draggable) {
        listItem->promptRebound();
        // animate panel only in case is called due to a signal emit
        listItem->initStyleItem();
        if (!panel && listItem->styleItem && listItem->styleItem->m_dragHandlerDelegate) {
            bool animate = (senderSignalIndex() >= 0);
            setupPanel(listItem->styleItem->m_dragHandlerDelegate, animate);
        }
        // install an event filter to catch mouse press events, as those are not
        // intercepted by the ListView filter
        if (panel) {
            // make panel to accept mouse buttons so we get mouse events over it
            panel->setAcceptedMouseButtons(Qt::LeftButton);
            panel->installEventFilter(this);
        }
    } else if (panel) {
        // disable mouse buttons from panel while not in drag mode
        panel->setAcceptedMouseButtons(Qt::NoButton);
        panel->removeEventFilter(this);
    }

    // update visuals
    listItem->update();
}

