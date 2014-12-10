
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>
#include <QtGui/QGuiApplication>
#include <QtQuick/private/qquickflickable_p.h>

UCDragHandler::UCDragHandler(UCListItem *listItem)
    : QObject(listItem)
    , listItem(UCListItemPrivate::get(listItem))
    , panel(0)
    , dragging(false)
    , isConnected(false)
{
    connect(this, &UCDragHandler::draggingChanged,
            listItem, &UCListItem::draggingChanged);
}

UCDragHandler::~UCDragHandler()
{
}

void UCDragHandler::setupDragPanel(bool animate)
{
    if (panel) {
        return;
    }
    // get the style loaded
    listItem->initStyleItem();
    if (listItem->styleItem && listItem->styleItem->m_dragHandlerDelegate) {
        if (listItem->styleItem->m_dragHandlerDelegate->isError()) {
            qmlInfo(listItem->q_ptr) << listItem->styleItem->m_dragHandlerDelegate->errorString();
        } else {
            UCListItem *item = listItem->item();
            // create a new context so we can expose context properties
            QQmlContext *context = new QQmlContext(qmlContext(item), item);
            context->setContextProperty("inDraggingMode",
                                        animate ? QVariant(false) : isDraggable());
            ContextPropertyChangeListener *listener = new ContextPropertyChangeListener(
                        context, "inDraggingMode");
            listener->setUpdaterProperty(listItem->attachedProperties, "draggable");

            panel = qobject_cast<QQuickItem*>(
                        listItem->styleItem->m_dragHandlerDelegate->beginCreate(context));
            if (panel) {
                QQml_setParent_noEvent(panel, item);
                panel->setParentItem(item);
                // complete component creation
                listItem->styleItem->m_dragHandlerDelegate->completeCreate();
                if (animate) {
                    // turn on draggable context property
                    context->setContextProperty("inDraggingMode", isDraggable());
                }
                // set left button as accepted so we can forward pressed events to the list view
                panel->setAcceptedMouseButtons(Qt::LeftButton);
            }
        }
    }
}

bool UCDragHandler::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress) {
        // forward to the ListView
        QQuickFlickable *listView = UCListItemAttachedPrivate::get(listItem->attachedProperties)->listView;
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        QPointF pos = listView->mapFromItem(static_cast<QQuickItem*>(watched), mouse->localPos());
        QMouseEvent mappedEvent(event->type(), pos, mouse->button(), mouse->buttons(), mouse->modifiers());
        QGuiApplication::sendEvent(listView, &mappedEvent);
    }
    return QObject::eventFilter(watched, event);
}

// listen for attached property's draggable change signal to activate dragging mode on the list item
void UCDragHandler::getNotified()
{
    if (!listItem->attachedProperties || isConnected) {
        return;
    }
    connect(listItem->attachedProperties, &UCListItemAttached::draggableChanged,
            this, &UCDragHandler::setupDragMode);
    // also connect ListItem's _q_enabler() slot to control content enabled based on selectable and draggable
    connect(listItem->attachedProperties, SIGNAL(draggableChanged()),
            listItem->item(), SLOT(_q_enabler()));
    isConnected = true;
}

bool UCDragHandler::isDraggable()
{
    UCListItemAttachedPrivate *attached = UCListItemAttachedPrivate::get(listItem->attachedProperties);
    return attached ? attached->isDraggable() : false;
}

void UCDragHandler::setupDragMode()
{
    // make sure the ListItem is snapped out
    bool draggable = isDraggable();
    if (draggable) {
        listItem->promptRebound();
        // animate panel only in case is called due to a signal emit
        bool animate = (senderSignalIndex() >= 0);
        setupDragPanel(animate);
        // stop children filtering while move mode is on
        listItem->item()->setFiltersChildMouseEvents(false);
        // install an event filter to catch mouse press events, as those are not
        // intercepted by the ListView filter
        if (panel) {
            panel->installEventFilter(this);
        }
    } else {
        listItem->item()->setFiltersChildMouseEvents(true);
        if (panel) {
            panel->removeEventFilter(this);
        }
    }

    // update visuals
    listItem->update();
}

