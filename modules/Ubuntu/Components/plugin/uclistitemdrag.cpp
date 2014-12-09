
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemstyle.h"
#include "listener.h"
#include <QtQml/QQmlInfo>
#include <QtQml/QQmlContext>

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
    if (!panel) {
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
                    // accept left mouse button to enable drag handling
                    panel->setAcceptedMouseButtons(Qt::LeftButton);
                    QQml_setParent_noEvent(panel, item);
                    panel->setParentItem(item);
                    // complete component creation
                    listItem->styleItem->m_dragHandlerDelegate->completeCreate();
                    if (animate) {
                        // turn on draggable context property
                        context->setContextProperty("inDraggingMode", isDraggable());
                    }
                }
            }
        }
    }
}

bool UCDragHandler::eventFilter(QObject *watched, QEvent *event)
{
    qDebug() << "EVENT" << event->type() << this;
    QEvent::Type type = event->type();
    bool mouseEvent = (type == QEvent::MouseButtonPress) ||
            (type == QEvent::MouseButtonRelease) ||
            (type == QEvent::MouseMove);

    QMouseEvent *mouse = mouseEvent ? static_cast<QMouseEvent*>(event) : 0;
    if (mouse) {
        if (type == QEvent::MouseButtonPress) {
            // lock flickables!
            listItem->attachedProperties->disableInteractive(listItem->item(), true);
            event->accept();
            return true;
        } else if (type == QEvent::MouseButtonRelease) {
            // unlock flickables!
            listItem->attachedProperties->disableInteractive(listItem->item(), false);
        }
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
        if (panel) {
            // install filter to catch mouse events
            panel->installEventFilter(this);
            // stop children filtering while move mode is on
            listItem->item()->setFiltersChildMouseEvents(false);
        }
    } else if (panel) {
        // remove filter and re-enable filtering on children events
        panel->removeEventFilter(this);
        listItem->item()->setFiltersChildMouseEvents(true);
    }

    listItem->contentItem->setEnabled(!draggable);
    // update visuals
    listItem->update();
}

