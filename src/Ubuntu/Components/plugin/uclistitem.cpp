/*
 * Copyright 2014-2015 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ucunits.h"
#include "uctheme.h"
#include "ucnamespace.h"
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemactions.h"
#include "uclistitemactions_p.h"
#include "ucubuntuanimation.h"
#include "propertychange_p.h"
#include "i18n.h"
#include "quickutils.h"
#include "ucaction.h"
#include "ucnamespace.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>
#include <QtGui/QGuiApplication>
#include <QtGui/QStyleHints>
#include <QtQuick/private/qquickanimation_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include "uclistitemstyle.h"
#include <QtQuick/private/qquickbehavior_p.h>
#include <QtQml/QQmlEngine>
#include <QFileInfo>
#include <QLibraryInfo>
#include "plugin.h"

/******************************************************************************
 * Divider
 */
class UCListItemDividerPrivate : public QQuickItemPrivate
{
    Q_DECLARE_PUBLIC(UCListItemDivider)
public:
    UCListItemDividerPrivate()
        : QQuickItemPrivate()
        , colorFromChanged(false)
        , colorToChanged(false)
        , listItem(0)
    {}

    bool colorFromChanged:1;
    bool colorToChanged:1;
    QColor colorFrom;
    QColor colorTo;
    QGradientStops gradient;
    UCListItem *listItem;
};

UCListItemDivider::UCListItemDivider(UCListItem *parent)
    : QQuickItem(*(new UCListItemDividerPrivate), parent)
{
    setFlag(ItemHasContents);
}
UCListItemDivider::~UCListItemDivider()
{
}

void UCListItemDivider::init(UCListItem *listItem)
{
    Q_D(UCListItemDivider);
    QQml_setParent_noEvent(this, listItem);
    d->listItem = listItem;
    setParentItem(listItem);
    // anchor to left/right/bottom of the ListItem
    QQuickAnchors *anchors = d->anchors();
    UCListItemPrivate *pListItem = UCListItemPrivate::get(listItem);
    anchors->setLeft(pListItem->left());
    anchors->setRight(pListItem->right());
    anchors->setBottom(pListItem->bottom());
    // connect visible change so we relayout contentItem
    // FIXME: do this with itemChange!!!
    connect(this, SIGNAL(visibleChanged()), listItem, SLOT(_q_relayout()));
}

void UCListItemDivider::paletteChanged()
{
    Q_D(UCListItemDivider);
    QColor background = d->listItem->getTheme()->getPaletteColor("normal", "background");
    if (!background.isValid()) {
        return;
    }
    // FIXME: we need a palette value for divider colors, till then base on the background
    // luminance
    if (!d->colorFromChanged || !d->colorToChanged) {
        qreal luminance = (background.red()*212 + background.green()*715 + background.blue()*73)/1000.0/255.0;
        bool lightBackground = (luminance > 0.85);
        if (!d->colorFromChanged) {
            d->colorFrom = lightBackground ? QColor("#26000000") : QColor("#26FFFFFF");
        }
        if (!d->colorToChanged) {
            d->colorTo = lightBackground ? QColor("#14FFFFFF") : QColor("#14000000");
        }
        updateGradient();
    }
}

void UCListItemDivider::updateGradient()
{
    Q_D(UCListItemDivider);
    d->gradient.clear();
    d->gradient.append(QGradientStop(0.0, d->colorFrom));
    d->gradient.append(QGradientStop(0.49, d->colorFrom));
    d->gradient.append(QGradientStop(0.5, d->colorTo));
    d->gradient.append(QGradientStop(1.0, d->colorTo));
    if (d->listItem) {
        d->listItem->update();
    }
}

QSGNode *UCListItemDivider::updatePaintNode(QSGNode *node, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    Q_D(UCListItemDivider);
    QSGRectangleNode *dividerNode = static_cast<QSGRectangleNode*>(node);
    if (!dividerNode) {
        dividerNode = d->sceneGraphContext()->createRectangleNode();
    }

    UCListItemPrivate *pListItem = UCListItemPrivate::get(d->listItem);
    bool lastItem = pListItem->countOwner ? (pListItem->index() == (pListItem->countOwner->property("count").toInt() - 1)): false;
    if (!lastItem && (d->gradient.size() > 0) && ((d->colorFrom.alphaF() >= (1.0f / 255.0f)) || (d->colorTo.alphaF() >= (1.0f / 255.0f)))) {
        dividerNode->setRect(boundingRect());
        dividerNode->setGradientStops(d->gradient);
        dividerNode->update();
        return dividerNode;
    } else if (node) {
        // delete the node
        delete node;
    }
    return 0;
}

QColor UCListItemDivider::colorFrom() const
{
    Q_D(const UCListItemDivider);
    return d->colorFrom;
}
void UCListItemDivider::setColorFrom(const QColor &color)
{
    Q_D(UCListItemDivider);
    if (d->colorFrom == color) {
        return;
    }
    d->colorFrom = color;
    d->colorFromChanged = true;
    updateGradient();
    Q_EMIT colorFromChanged();
}

QColor UCListItemDivider::colorTo() const
{
    Q_D(const UCListItemDivider);
    return d->colorTo;
}
void UCListItemDivider::setColorTo(const QColor &color)
{
    Q_D(UCListItemDivider);
    if (d->colorTo == color) {
        return;
    }
    d->colorTo = color;
    d->colorToChanged = true;
    updateGradient();
    Q_EMIT colorToChanged();
}

/******************************************************************************
 * ListItemPrivate
 */
UCListItemPrivate::UCListItemPrivate()
    : UCStyledItemBasePrivate()
    , color(Qt::transparent)
    , highlightColor(Qt::transparent)
    , contentItem(new QQuickItem)
    , divider(new UCListItemDivider)
    , leadingActions(Q_NULLPTR)
    , trailingActions(Q_NULLPTR)
    , mainAction(Q_NULLPTR)
    , expansion(Q_NULLPTR)
    , xAxisMoveThresholdGU(DEFAULT_SWIPE_THRESHOLD_GU)
    , button(Qt::NoButton)
    , highlighted(false)
    , contentMoved(false)
    , swiped(false)
    , suppressClick(false)
    , ready(false)
    , customColor(false)
{
}
UCListItemPrivate::~UCListItemPrivate()
{
}

void UCListItemPrivate::init()
{
    Q_Q(UCListItem);
    contentItem->setObjectName("ListItemHolder");
    QQml_setParent_noEvent(contentItem, q);
    contentItem->setParentItem(q);
    contentItem->setClip(true);
    divider->init(q);
    // content will be redirected to the contentItem, therefore we must report
    // children changes as it would come from the main component
    QObject::connect(contentItem, &QQuickItem::childrenChanged,
                     q, &UCListItem::listItemChildrenChanged);
    q->setFlag(QQuickItem::ItemHasContents);
    // turn activeFocusOnPress on
    q->setActiveFocusOnPress(true);

    // update swiped state
    QObject::connect(contentItem, SIGNAL(xChanged()),
                     q, SLOT(_q_updateSwiping()), Qt::DirectConnection);

    // connect theme changes
    QObject::connect(q, SIGNAL(themeChanged()),
                     q, SLOT(_q_themeChanged()), Qt::DirectConnection);

    // watch grid unit size change and set implicit size
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateSize()));
    _q_updateSize();
    setStyleName("ListItemStyle");
}

void UCListItemPrivate::_q_themeChanged()
{
    // call the post theme changes
    postThemeChanged();
}

// inspired from IS_SIGNAL_CONNECTED(q, UCListItem, pressAndHold, ())
// the macro cannot be used due to Arguments cannot be an empty ()
bool UCListItemPrivate::isClickedConnected()
{
    Q_Q(UCListItem);
    static QMetaMethod method = QMetaMethod::fromSignal(&UCListItem::clicked);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(q)->isSignalConnected(signalIdx);
}

bool UCListItemPrivate::isPressAndHoldConnected()
{
    Q_Q(UCListItem);
    static QMetaMethod method = QMetaMethod::fromSignal(&UCListItem::pressAndHold);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(q)->isSignalConnected(signalIdx);
}

void UCListItemPrivate::postThemeChanged()
{
    Q_Q(UCListItem);
    // update divider colors
    divider->paletteChanged();

    // update colors, panels
    if (!customColor) {
        q->resetHighlightColor();
    }
}

// re-layouting the ListItem's contentItem
void UCListItemPrivate::_q_relayout()
{
    QQuickAnchors *contentAnchors = QQuickItemPrivate::get(contentItem)->anchors();
    QQuickAnchorLine anchorLine;
    if (divider->isVisible()) {
        anchorLine = QQuickItemPrivate::get(divider)->top();
    } else {
        anchorLine = bottom();
    }
    contentAnchors->setBottom(anchorLine);
}

void UCListItemPrivate::_q_updateIndex()
{
    Q_Q(UCListItem);
    q->update();
}

// update contentMoving property when ListItemStyle.snapAnimation stopped() signal is emitted
void UCListItemPrivate::_q_contentMoving()
{
    setContentMoving(listItemStyle()->m_snapAnimation->isRunning());
}

// synchronizes selection mode, initializes the style if has not been done yet,
// which in turn reveals the selection panels
void UCListItemPrivate::_q_syncSelectMode()
{
    loadStyleItem();
    Q_Q(UCListItem);
    Q_EMIT q->selectModeChanged();
}

// same for the dragMode
void UCListItemPrivate::_q_syncDragMode()
{
    loadStyleItem();
    Q_Q(UCListItem);
    Q_EMIT q->dragModeChanged();
}

void UCListItemPrivate::preStyleChanged()
{
    snapOut();
    UCStyledItemBasePrivate::preStyleChanged();
}

// creates the style item, with altered default value of the animatePanels style property
// the property is turned on after the panel initialization.
bool UCListItemPrivate::loadStyleItem(bool animated)
{
    // the style should be loaded only if one of the condition is satisfied
    if (!swiped && !selectMode() && !dragMode() && !(expansion && expansion->expanded())) {
        return false;
    }

    if (!UCStyledItemBasePrivate::loadStyleItem(animated)) {
        return false;
    }

    UCListItemStyle *myStyle = qobject_cast<UCListItemStyle*>(styleItem);
    if (!myStyle) {
        // the style is not derived from ListItemStyle, clean
        preStyleChanged();
        return false;
    }
    myStyle->updateFlickable(flickable);
    // bring the panels foreground
    styleItem->setZ(0);
    listItemStyle()->setAnimatePanels(true);
    return true;
}

// called when units size changes
void UCListItemPrivate::_q_updateSize()
{
    Q_Q(UCListItem);
    // update divider thickness
    divider->setImplicitHeight(UCUnits::instance().dp(DIVIDER_THICKNESS_DP));
    QQuickItem *owner = qobject_cast<QQuickItem*>(q->sender());
    if (!owner && parentAttached) {
        owner = static_cast<QQuickItem*>(parentAttached->parent());
    }
    q->setImplicitWidth(owner ? owner->width() : UCUnits::instance().gu(IMPLICIT_LISTITEM_WIDTH_GU));
    q->setImplicitHeight(UCUnits::instance().gu(IMPLICIT_LISTITEM_HEIGHT_GU));
}

// returns the index of the list item when used in model driven views,
// and the child index in other cases
int UCListItemPrivate::index()
{
    Q_Q(UCListItem);
    // is there an index context property?
    QQmlContext *context = qmlContext(q);
    QVariant index = context->contextProperty("index");
    return index.isValid() ?
                index.toInt() :
                (parentItem ? QQuickItemPrivate::get(parentItem)->childItems.indexOf(q) : -1);
}

// returns true if the highlight is possible; the highlight is possible if the
// list item has at least one action, leading/trailing actions set, onClicked
// or onPressAndHold signal handlers set
bool UCListItemPrivate::canHighlight()
{
   return (isClickedConnected() || isPressAndHoldConnected() || mainAction || leadingActions || trailingActions);
}

// set highlighted flag and update contentItem
void UCListItemPrivate::setHighlighted(bool highlighted)
{
    if (this->highlighted != highlighted) {
        this->highlighted = highlighted;
        suppressClick = false;
        Q_Q(UCListItem);
        q->update();
        if (highlighted) {
            // start pressandhold timer
            pressAndHoldTimer.start(QGuiApplication::styleHints()->mousePressAndHoldInterval(), q);
        } else {
            pressAndHoldTimer.stop();
        }
        Q_EMIT q->highlightedChanged();
    }
}
// toggles the swiped flag and installs/removes event filter to capture pointer events outside
// of list item area
void UCListItemPrivate::_q_updateSwiping()
{
    if (swiped) {
        setSwiped((contentItem->position() != zeroPos) || highlighted);
    }
}

void UCListItemPrivate::setSwiped(bool swiped)
{
    suppressClick = swiped;
    if (this->swiped == swiped) {
        return;
    }
    this->swiped = swiped;
    Q_Q(UCListItem);
    QQuickWindow *window = q->window();
    if (swiped) {
        window->installEventFilter(q);
    } else {
        window->removeEventFilter(q);
        // lock contentItem left/right edges
        lockContentItem(true);
    }
}

// connects/disconnects from the Flickable anchestor to get notified when to do rebound
void UCListItemPrivate::listenToRebind(bool listen)
{
    if (parentAttached) {
        Q_Q(UCListItem);
        parentAttached->listenToRebind(q, listen);
    }
}

// lock/unlock contentItem's left and right anchors to the ListItem's left and right
void UCListItemPrivate::lockContentItem(bool lock)
{
    QQuickAnchors *contentAnchors = QQuickItemPrivate::get(contentItem)->anchors();
    if (lock) {
        contentAnchors->setLeft(left());
        contentAnchors->setRight(right());
        zeroPos = contentItem->position();
    } else {
        contentAnchors->resetLeft();
        contentAnchors->resetRight();
    }
}

void UCListItemPrivate::update()
{
    if (!ready) {
        return;
    }
    Q_Q(UCListItem);
    q->update();
}

/*
 * Snap out is performed when the ListItem.contentItem returns back to its original
 * X coordinates (0). At this point both leading and trailing action panels will
 * be disconnected, ascending Flickables will get unlocked (interactive value restored
 * to the state before they were locked) and ListItem.contentMoving will be reset.
 */
void UCListItemPrivate::snapOut()
{
    if (!ready) {
        return;
    }
    setHighlighted(false);
    if (parentAttached) {
        Q_Q(UCListItem);
        // restore flickable's interactive and cleanup
        q->setKeepMouseGrab(false);
        // no need to listen flickables any longer
        listenToRebind(false);
    }
    if (styleItem) {
        listItemStyle()->invokeRebound();
    }
}

// emits the style signal swipeEvent()
void UCListItemPrivate::swipeEvent(const QPointF &localPos, UCSwipeEvent::Status status)
{
    UCSwipeEvent event(localPos, lastPos, contentItem->position() + (localPos - lastPos), status);
    // clamp to the edges if the edge (leading/trailing) doesn't have actions defined
    if ((event.m_contentPos.x() < zeroPos.x() && !trailingActions) ||
        (event.m_contentPos.x() > zeroPos.x() && !leadingActions)) {
        event.m_contentPos = zeroPos;
    }
    if (styleItem) {
        listItemStyle()->invokeSwipeEvent(&event);
    }
    if (event.m_contentPos != contentItem->position()) {
        contentItem->setPosition(event.m_contentPos);
        lastPos = localPos;
        if (status == UCSwipeEvent::Updated) {
            setContentMoving(true);
            setSwiped(true);
        }
    }
}

/*!
 * \qmltype ListItem
 * \instantiates UCListItem
 * \inqmlmodule Ubuntu.Components 1.2
 * \ingroup ubuntu-listitem
 * \since Ubuntu.Components 1.2
 * \brief The ListItem element provides Ubuntu design standards for list or grid
 * views.
 * The ListItem component was designed to be used in a list view. It does not
 * define any specific layout, but while its contents can be freely chosen by
 * the developer, care must be taken to keep the contents light in order to ensure
 * good performance when used in long list views.
 *
 * The component provides two color properties which configures the item's background
 * when normal or highlighted. This can be configured through \l color and \l highlightColor
 * properties. The list item is highlighted if there is an action attached to it.
 * This means that the list item must have an active component declared as child,
 * at least leading- or trailing actions specified, or to have a slot connected to
 * \l clicked or \l pressAndHold signal. In any other case the component will not
 * be highlighted, and \l highlighted property will not be toggled either. Also,
 * there will be no highlight happening if the click happens on the active component.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * MainView {
 *    width: units.gu(40)
 *    height: units.gu(71)
 *
 *    Column {
 *        anchors.fill: parent
 *        ListItem {
 *            Button {
 *                text: "Press me"
 *            }
 *            onClicked: console.log("clicked on ListItem")
 *        }
 *        ListItem {
 *            leadingActions: ListItemActions {
 *                actions: [
 *                    Action {
 *                        iconName: "delete"
 *                    }
 *                ]
 *            }
 *            onClicked: console.log("clicked on ListItem with leadingActions set")
 *        }
 *        ListItem {
 *            trailingActions: ListItemActions {
 *                actions: [
 *                    Action {
 *                        iconName: "edit"
 *                    }
 *                ]
 *            }
 *            onClicked: console.log("clicked on ListItem with trailingActions set")
 *        }
 *        ListItem {
 *            Label {
 *                text: "onClicked implemented"
 *            }
 *            onClicked: console.log("clicked on ListItem with onClicked implemented")
 *        }
 *        ListItem {
 *            Label {
 *                text: "onPressAndHold implemented"
 *            }
 *            onPressAndHold: console.log("long-pressed on ListItem with onPressAndHold implemented")
 *        }
 *        ListItem {
 *            Label {
 *                text: "No highlight"
 *            }
 *        }
 *    }
 * }
 * \endqml
 *
 * \l contentItem holds all components and resources declared as child to ListItem.
 * Being an Item, all properties can be accessed or altered. However, make sure you
 * never change \b x, \b y, \b width, \b height or \b anchors properties as those are
 * controlled by the ListItem itself when leading or trailing actions are revealed
 * or when selectable and draggable mode is turned on, and thus might cause the
 * component to misbehave. Anchors margins are free to alter.
 *
 * Each ListItem has a thin divider shown on the bottom of the component. This
 * divider can be configured through the \l divider grouped property, which can
 * configure its margins from the edges of the ListItem as well as its visibility.
 * When used in ListView or \l UbuntuListView, the last list item will not
 * show the divider no matter of the visible property value set.
 *
 * ListItem can handle actions that can get swiped from front or back of the item.
 * These actions are Action elements visualized in panels attached to the front
 * or to the back of the item, and are revealed by swiping the item horizontally.
 * The swipe is started only after the mouse/touch move had passed a given threshold.
 * The actions are visualized by a panel, which is configurable through the \l
 * ListItemStyle.
 *
 * The actions are configured through the \l leadingActions as well as \l
 * trailingActions properties.
 * \qml
 * ListItem {
 *     id: listItem
 *     leadingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "delete"
 *                 onTriggered: listItem.destroy()
 *             }
 *         ]
 *     }
 *     trailingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "search"
 *                 onTriggered: {
 *                     // do some search
 *                 }
 *             }
 *         ]
 *     }
 * }
 * \endqml
 * \note When a list item is swiped, it automatically connects both leading and
 * trailing actions to the list item. If needed, the same ListItemActions instance
 * can be used in both leading and trailing side. In the following example the
 * list item can be deleted through both leading and trailing actions using the
 * same container:
 * \qml
 * ListItem {
 *     id: listItem
 *     leadingActions: ListItemActions {
 *         actions: [
 *             Action {
 *                 iconName: "delete"
 *                 onTriggered: listItem.destroy()
 *             }
 *         ]
 *     }
 *     trailingActions: leadingActions
 * }
 * \endqml
 * The action is triggered only after all the animations are completed.
 * \sa ListItemActions
 *
 * ListItem provides a set of attached properties which are attached to each panel
 * of the ListItem. However not all properties are valid in all the circumstances.
 *
 * The component is styled using the \l ListItemStyle style interface.
 *
 * \section2 Selection mode
 * The selection mode of a ListItem is controlled by the \l ViewItems::selectMode
 * attached property. This property is attached to each parent item of the ListItem
 * exception being when used as delegate in ListView, where the property is attached
 * to the view itself.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * Flickable {
 *    width: units.gu(40)
 *    height: units.gu(50)
 *
 *    // this will not have any effect
 *    ViewItems.selectMode: true
 *    Column {
 *        // this will work
 *        ViewItems.selectMode: false
 *        width: parent.width
 *        Repeater {
 *            model: 25
 *            ListItem {
 *                Label {
 *                    text: "ListItem in Flickable #" + index
 *                }
 *            }
 *        }
 *    }
 * }
 * \endqml
 * The indices selected are stored in \l ViewItems::selectedIndices attached property,
 * attached the same way as the \l ViewItems::selectMode property is. This is a
 * read/write property, meaning that initial selected item indices can be set up.
 * The list contains the indices added in the order of selection, not sorted in
 * any form.
 *
 * \note When in selectable mode, the ListItem content is not disabled and \l clicked
 * and \l pressAndHold signals are also emitted. The only restriction the component
 * implies is that leading and trailing actions cannot be swiped in. \ selectable
 * property can be used to implement different behavior when \l clicked or \l
 * pressAndHold.
 *
 * \section2 Dragging mode
 * The dragging mode is only supported on ListView, as it requires a model supported
 * view to be used. The drag mode can be activated through the \l ViewItems::dragMode
 * attached property, when attached to the ListView. The items will show a panel
 * as defined in the style, and dragging will be possible when initiated over this
 * panel. Pressing or clicking anywhere else on the ListItem will invoke the item's
 * action assigned to the touched area.
 *
 * The dragging is realized through the \l ViewItems::dragUpdated signal, and
 * a signal handler must be implemented in order to have the draging working.
 * Implementations can drive the drag to be live (each time the dragged item is
 * dragged over an other item will change the order of the items) or drag'n'drop
 * way (the dragged item will be moved only when the user releases the item by
 * dropping it to the desired position). The signal has a \l ListItemDrag \e event
 * parameter, which gives detailed information about the drag event, like started,
 * dragged up or downwards or dropped, allowing in this way various restrictions
 * on the dragging.
 *
 * The dragging event provides three states reported in \l ListItemDrag::status
 * field, \e Started, \e Moving and \e Dropped. The other event field values depend
 * on the status, therefore the status must be taken into account when implementing
 * the signal handler. In case live dragging is needed, \e Moving state must be
 * checked, and for non-live drag (drag'n'drop) the \e Moving state must be blocked
 * by setting \e {event.accept = false}, otherwise the dragging will not know
 * whether the model has been updated or not.
 *
 * Example of live drag implementation:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *     model: ListModel {
 *         Component.onCompleted: {
 *             for (var i = 0; i < 100; i++) {
 *                 append({tag: "List item #"+i});
 *             }
 *         }
 *     }
 *     delegate: ListItem {
 *         Label {
 *             text: modelData
 *         }
 *         color: dragMode ? "lightblue" : "lightgray"
 *         onPressAndHold: ListView.view.ViewItems.dragMode =
 *             !ListView.view.ViewItems.dragMode
 *     }
 *     ViewItems.onDragUpdated: {
 *         if (event.status == ListItemDrag.Moving) {
 *             model.move(event.from, event.to, 1);
 *         }
 *     }
 *     moveDisplaced: Transition {
 *         UbuntuNumberAnimation {
 *             property: "y"
 *         }
 *     }
 * }
 * \endqml
 *
 * Example of drag'n'drop implementation:
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *     model: ListModel {
 *         Component.onCompleted: {
 *             for (var i = 0; i < 100; i++) {
 *                 append({tag: "List item #"+i});
 *             }
 *         }
 *     }
 *     delegate: ListItem {
 *         Label {
 *             text: modelData
 *         }
 *         color: dragMode ? "lightblue" : "lightgray"
 *         onPressAndHold: ListView.view.ViewItems.dragMode =
 *             !ListView.view.ViewItems.dragMode
 *     }
 *     ViewItems.onDragUpdated: {
 *         if (event.status == ListItemDrag.Moving) {
 *             // inform dragging that move is not performed
 *             event.accept = false;
 *         } else if (event.status == ListItemDrag.Dropped) {
 *             model.move(event.from, event.to, 1);
 *         }
 *     }
 *     moveDisplaced: Transition {
 *         UbuntuNumberAnimation {
 *             property: "y"
 *         }
 *     }
 * }
 * \endqml
 *
 * ListItem does not provide animations when the ListView's model is updated. In order
 * to have animation, use UbuntuListView or provide a transition animation to the
 * moveDisplaced or displaced property of the ListView.
 *
 * \section3 Using non-QAbstractItemModel models
 * Live dragging (moving content on the move) is only possible when the model is
 * a derivate of the QAbstractItemModel. When a list model is used, the ListView
 * will re-create all the items in the view, meaning that the dragged item will
 * no longer be controlled by the dragging. However, non-live drag'n'drop operations
 * can still be implemented with these kind of lists as well.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *     model: ["plum", "peach", "pomegrenade", "pear", "banana"]
 *     delegate: ListItem {
 *         Label {
 *             text: modelData
 *         }
 *         color: dragMode ? "lightblue" : "lightgray"
 *         onPressAndHold: ListView.view.ViewItems.dragMode =
 *             !ListView.view.ViewItems.dragMode
 *     }
 *     ViewItems.onDragUpdated: {
 *         if (event.status == ListItemDrag.Started) {
 *             return;
 *         } else if (event.status == ListItemDrag.Dropped) {
 *             var fromData = model[event.from];
 *             // must use a temporary variable as list manipulation
 *             // is not working directly on model
 *             var list = model;
 *             list.splice(event.from, 1);
 *             list.splice(event.to, 0, fromData);
 *             model = list;
 *         } else {
 *             event.accept = false;
 *         }
 *     }
 * }
 * \endqml
 *
 * When using DelegateModel, it must be taken into account when implementing the
 * \l ViewItems::dragUpdated signal handler.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * ListView {
 *     model: DelegateModel {
 *         model: ["apple", "pear", "plum", "peach", "nuts", "dates"]
 *         delegate: ListItem {
 *             Label {
 *                 text: modelData
 *             }
 *             onPressAndHold: dragMode = !dragMode;
 *         }
 *     }
 *     ViewItems.onDragUpdated: {
 *         if (event.status == ListItemDrag.Moving) {
 *             event.accept = false
 *         } else if (event.status == ListItemDrag.Dropped) {
 *             var fromData = model.model[event.from];
 *             var list = model.model;
 *             list.splice(event.from, 1);
 *             list.splice(event.to, 0, fromData);
 *             model.model = list;
 *         }
 *     }
 * }
 * \endqml
 *
 * \sa ViewItems::dragMode, ViewItems::dragUpdated
 *
 * \section2 Expansion
 * Since Ubuntu.Components 1.3, ListItem supports expansion. ListItems declared
 * in a view can expand exclusively, having leading and trailing panes locked
 * when expanded and to be collapsed when tapping outside of the expanded area.
 * The expansion is driven by the \l expansion group property, and the behavior
 * by the \l ViewItems::expansionFlags and \l ViewItems::expandedIndices
 * attached properties. Each ListItem which is required to expand should set a
 * proper height in the \l expansion.height property, which should be bigger
 * than the collapsed height of the ListItem is. The expansion itself is driven
 * by the \l expansion.expanded property, which can be set freely depending on
 * the use case, on click, on long press, etc.
 *
 * The default expansion behavior is set to be exclusive and locked, meaning
 * there can be only one ListItem expanded within a view and neither leading
 * nor trailing action panels cannot be swiped in. Expanding an other ListItem
 * will collapse the previosuly expanded one. There can be cases when tapping
 * outside of the expanded area of a ListItem we woudl need the expanded one
 * to collapse automatically. This can be achieved by setting \c ViewItems.CollapseOnOutsidePress
 * flag to \l ViewItems::expansionFlags. This flag will also turn on \c ViewItems.Exclusive
 * flag, as tapping outside practicly forbids more than one item to be expanded
 * at a time.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.3
 *
 * ListView {
 *     width: units.gu(40)
 *     height: units.gu(71)
 *     model: ListModel {
 *         Component.onCompleted: {
 *             for (var i = 0; i < 50; i++) {
 *                 append({data: i});
 *             }
 *         }
 *     }
 *     ViewItems.expansionFlags: ViewItems.CollapseOnOutsidePress
 *     delegate: ListItem {
 *         Label {
 *             text: "Model item #" + modelData
 *         }
 *         trailingActions: ListItemActions {
 *             actions: [
 *                 Action {
 *                     icon: "search"
 *                 },
 *                 Action {
 *                     icon: "edit"
 *                 },
 *                 Action {
 *                     icon: "copy"
 *                 }
 *             ]
 *         }
 *         expansion.height: units.gu(15)
 *         onClicked: expansion.expanded = true
 *     }
 * }
 * \endqml
 * The example above collapses the expanded item whenever it is tapped or mouse
 * pressed outside of the expanded list item.
 * \note Set 0 to \l ViewItems::expansionFlags if no restrictions on expanded items
 * is required (i.e multiple expanded items are allowed, swiping leading/trailing
 * actions when expanded).
 * \note Do not bind \l expansion.height to the ListItem's height as is will cause
 * binding loops.
 *
 * \section2 Note on styling
 * ListItem's styling differs from the other components styling, as ListItem loads
 * the style only when either of the leadin/trailing panels are swiped, or when the
 * item enters in select- or drag mode. The component does not assume any visuals
 * to be present in the style.
 * \sa ListItemStyle
 */

/*!
 * \qmlsignal ListItem::clicked()
 * The signal is emitted when the component gets released while the \l highlighted property
 * is set. The signal is not emitted if the ListItem content is swiped or when used in
 * Flickable (or ListView, GridView) and the Flickable gets moved.
 *
 * If the ListItem contains a component which contains an active MouseArea, the clicked
 * signal will be supressed when clicked over this area.
 */

/*!
 * \qmlsignal ListItem::pressAndHold()
 * The signal is emitted when the list item is long pressed.
 *
 * If the ListItem contains a component which contains an active MouseArea, the pressAndHold
 * signal will be supressed when pressed over this area.
 */

UCListItem::UCListItem(QQuickItem *parent)
    : UCStyledItemBase(*(new UCListItemPrivate), parent)
{
    Q_D(UCListItem);
    d->init();
}

UCListItem::~UCListItem()
{
}

QObject *UCListItem::attachedViewItems(QObject *object, bool create)
{
    return qmlAttachedPropertiesObject<UCViewItemsAttached>(object, create);
}

void UCListItem::classBegin()
{
    UCStyledItemBase::classBegin();
    Q_D(UCListItem);
    d->_q_themeChanged();
    d->divider->paletteChanged();
}

void UCListItem::componentComplete()
{
    UCStyledItemBase::componentComplete();
    Q_D(UCListItem);
    // set contentItem's context
    QQmlEngine::setContextForObject(d->contentItem, qmlContext(this));
    // anchor contentItem prior doing anything else
    QQuickAnchors *contentAnchors = QQuickItemPrivate::get(d->contentItem)->anchors();
    contentAnchors->setTop(d->top());
    d->_q_relayout();
    d->lockContentItem(true);

    d->ready = true;
    /* We only deal with ListView, as for other cases we would need to check the children
     * changes, which would have an enormous impact on performance in case of huge amount
     * of items. However, if the parent item, or Flickable declares a "count" property,
     * the ListItem will take use of it!
     */
    d->countOwner = (d->flickable && d->flickable->property("count").isValid()) ?
                d->flickable :
                (d->parentItem && d->parentItem->property("count").isValid()) ? d->parentItem : 0;
    if (d->countOwner) {
        QObject::connect(d->countOwner.data(), SIGNAL(countChanged()),
                         this, SLOT(_q_updateIndex()), Qt::DirectConnection);
        update();
    }

    if (d->parentAttached) {
        // connect selectedIndicesChanged
        connect(d->parentAttached.data(), &UCViewItemsAttached::selectedIndicesChanged,
                this, &UCListItem::selectedChanged);
        // sync selectModeChanged()
        connect(d->parentAttached, SIGNAL(selectModeChanged()),
                this, SLOT(_q_syncSelectMode()));
        // also draggable
        connect(d->parentAttached, SIGNAL(dragModeChanged()),
                this, SLOT(_q_syncDragMode()));

        // if selection or drag mode is on, initialize style, with animations turned off
        if (d->parentAttached->selectMode() || d->parentAttached->dragMode() || (d->expansion && d->expansion->expanded())) {
            d->loadStyleItem(false);
        }
        // set the object name for testing purposes
        if (d->dragging()) {
            setObjectName("DraggedListItem");
        }
    }
}

void UCListItem::itemChange(ItemChange change, const ItemChangeData &data)
{
    UCStyledItemBase::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        Q_D(UCListItem);
        // make sure we are not connected to any previous Flickable
        d->listenToRebind(false);
        // check if we are in a positioner, and if that positioner is in a Flickable
        QQuickBasePositioner *positioner = qobject_cast<QQuickBasePositioner*>(data.item);
        if (positioner && positioner->parentItem()) {
            d->flickable = qobject_cast<QQuickFlickable*>(positioner->parentItem()->parentItem());
        } else if (data.item && data.item->parentItem()){
            // check if we are in a Flickable then
            d->flickable = qobject_cast<QQuickFlickable*>(data.item->parentItem());
        }

        // attach ViewItems to parent item or to ListView
        QQuickItem *parentAttachee = data.item;
        if (d->flickable && d->flickable->inherits("QQuickListView")) {
            // attach to ListView
            d->parentAttached = static_cast<UCViewItemsAttached*>(attachedViewItems(d->flickable, true));
            parentAttachee = d->flickable;
        } else if (data.item) {
            d->parentAttached = static_cast<UCViewItemsAttached*>(attachedViewItems(data.item, true));
        } else {
            // mark as not ready, so no action should be performed which depends on readyness
            d->ready = false;
            // about to be deleted or reparented, disable attached
            d->parentAttached = 0;
        }

        if (d->styleItem) {
            UCListItemStyle * myStyle = static_cast<UCListItemStyle*>(d->styleItem);
            myStyle->updateFlickable(d->flickable);
        }

        if (d->parentAttached) {
            connect(d->parentAttached.data(), SIGNAL(expandedIndicesChanged(QList<int>)),
                    this, SLOT(_q_updateExpansion(QList<int>)), Qt::DirectConnection);
        }

        if (parentAttachee) {
            QObject::connect(parentAttachee, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()), Qt::DirectConnection);
            // update size
            d->_q_updateSize();
        }
    }
}

QSGNode *UCListItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    Q_D(UCListItem);
    QColor color = d->highlighted ? d->highlightColor : d->color;

    if (width() <= 0 || height() <= 0) {
        delete oldNode;
        return 0;
    }

    QSGRectangleNode *rectNode = 0;
    rectNode = static_cast<QSGRectangleNode*>(oldNode);
    if (!rectNode) {
        rectNode = QQuickItemPrivate::get(this)->sceneGraphContext()->createRectangleNode();
    }
    if (color.alphaF() >= (1.0f / 255.0f)) {
        rectNode->setColor(color);
        // cover only the area of the contentItem, removing divider's thickness
        QRectF rect(boundingRect());
        if (d->divider->isVisible()) {
            rect -= QMarginsF(0, 0, 0, d->divider->height());
        }
        rectNode->setRect(rect);
        rectNode->setGradientStops(QGradientStops());
        rectNode->setAntialiasing(true);
        rectNode->setAntialiasing(false);
        rectNode->update();
    } else {
        // delete node, this will delete the divider node as well
        delete rectNode;
        rectNode = 0;
    }
    oldNode = rectNode;
    return oldNode;
}

// grabs the left mouse button event by turning highlight on, and triggering
// swipe events; child items should no longer get mouse events
void UCListItemPrivate::handleLeftButtonPress(QMouseEvent *event)
{
    Q_Q(UCListItem);
    button = event->button();
    // create style instance
    loadStyleItem();
    setHighlighted(true);
    lastPos = pressedPos = event->localPos();
    // connect the Flickable to know when to rebound
    listenToRebind(true);
    if (swiped) {
        // grab now, and ungrab in snapOut
        q->setKeepMouseGrab(true);
        q->grabMouse();
    }
    // stop any ongoing animation!
    swipeEvent(event->localPos(), UCSwipeEvent::Started);
    // accept the event so we get the rest of the events as well
    event->accept();
}

void UCListItem::mousePressEvent(QMouseEvent *event)
{
    UCStyledItemBase::mousePressEvent(event);
    Q_D(UCListItem);
    d->button = event->button();
    if (d->parentAttached && d->parentAttached->isMoving()) {
        // while moving, we cannot select any items
        return;
    }
    if (d->canHighlight() && !d->highlighted && event->button() == Qt::LeftButton) {
        d->handleLeftButtonPress(event);
    }
    if (d->shouldShowContextMenu(event)) {
        d->showContextMenu();
    }
}

bool UCListItemPrivate::shouldShowContextMenu(QMouseEvent *event)
{
    if (event->button() != Qt::RightButton)
        return false;
    return leadingActions || trailingActions;
}

void UCListItemPrivate::_q_popoverClosed()
{
    setHighlighted(false);
}

void UCListItemPrivate::showContextMenu()
{
    Q_Q(UCListItem);
    // themes 1.2 and below should not have context menu support, so leave
    quint16 version(importVersion(q));
    if (version <= BUILD_VERSION(1, 2)) {
        return;
    }

    // Highlight the Item while the menu is showing
    setHighlighted(true);
    // Reset the timer which otherwise is started with highlighting
    pressAndHoldTimer.stop();

    QString versionString(QStringLiteral("%1.%2").arg(MAJOR_VERSION(version)).arg(MINOR_VERSION(version)));
    const QString relativeUrl = versionString + "/ListItemPopover.qml";
    QUrl url(UbuntuComponentsPlugin::pluginUrl().resolved(relativeUrl));

    // Open Popover
    QQmlEngine* engine = qmlEngine(q);
    QQmlComponent* component = new QQmlComponent(engine, url, QQmlComponent::PreferSynchronous, q);
    if (component->isError()) {
        qmlInfo(q) << component->errorString();
    } else {
        QQmlEngine::setContextForObject(component, qmlContext(q));
        QQuickItem* item = static_cast<QQuickItem*>(component->create(qmlContext(q)));
        item->setProperty("caller", QVariant::fromValue(q));
        item->setParentItem(QuickUtils::instance().rootItem(q));
        QMetaObject::invokeMethod(item, "show");
        QObject::connect(item, SIGNAL(visibleChanged()), q,
            SLOT(_q_popoverClosed()), Qt::DirectConnection);
    }
    delete component;
}

// ungrabs any previously grabbed left mouse button event
void UCListItemPrivate::handleLeftButtonRelease(QMouseEvent *event)
{
    Q_Q(UCListItem);
    // set released
    if (highlighted) {
        // unblock ascending Flickables
        listenToRebind(false);
        q->setKeepMouseGrab(false);
        if (window && window->mouseGrabberItem() == q) {
            q->ungrabMouse();
        }

        if (!suppressClick) {
            // emit clicked only if not swiped
            if (!swiped) {
                Q_EMIT q->clicked();
                if (mainAction) {
                    Q_EMIT mainAction->trigger(index());
                }
            }
            snapOut();
        } else {
            // inform style about mouse/touch release
            swipeEvent(event->localPos(), UCSwipeEvent::Finished);
            suppressClick = false;
            setHighlighted(false);
        }
    }
    button = Qt::NoButton;
    event->accept();
}

void UCListItem::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(UCListItem);
    if (d->shouldShowContextMenu(event)) {
        return;
    }

    UCStyledItemBase::mouseReleaseEvent(event);
    d->handleLeftButtonRelease(event);
}

// returns true if the mouse is swiped over the threshold value
bool UCListItemPrivate::swipedOverThreshold(const QPointF &mousePos, const QPointF relativePos)
{
    qreal threshold = UCUnits::instance().gu(xAxisMoveThresholdGU);
    qreal mouseX = mousePos.x();
    qreal pressedX = relativePos.x();
    return ((mouseX < (pressedX - threshold)) || (mouseX > (pressedX + threshold)));
}

void UCListItem::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(UCListItem);
    UCStyledItemBase::mouseMoveEvent(event);

    if (d->selectMode() || d->dragMode() || (d->expansion && d->expansion->expandedLocked())) {
        // no move is allowed while selectable mode is on
        return;
    }

    // accept the tugging only if the move is within the threshold
    // use saved button because MouseMove has no button() and buttons() isn't reliable
    if (d->button == Qt::LeftButton && d->highlighted && !d->swiped && (d->leadingActions || d->trailingActions)) {
        // check if we can initiate the drag at all
        // only X direction matters, if Y-direction leaves the threshold, but X not, the tug is not valid
        if (d->swipedOverThreshold(event->localPos(), d->pressedPos)) {
            // the press went out of the threshold area, enable move, if the direction allows it
            d->lastPos = event->localPos();
            setKeepMouseGrab(true);
            qreal mouseX = event->localPos().x();
            qreal pressedX = d->pressedPos.x();
            bool doSwipe = (d->leadingActions && (mouseX > pressedX)) ||
                           (d->trailingActions && (mouseX < pressedX));
            d->setSwiped(doSwipe);
            // unlock contentItem's left/right edges
            d->lockContentItem(!doSwipe);
            d->loadStyleItem();
            d->pressAndHoldTimer.stop();
        }
    }

    if (d->swiped) {
        d->pressAndHoldTimer.stop();

        // send swipe event to style and update contentItem position
        d->swipeEvent(event->localPos(), UCSwipeEvent::Updated);
    }
}

bool UCListItemPrivate::sendMouseEvent(QQuickItem *item, QMouseEvent *event)
{
    Q_UNUSED(item);
    Q_Q(UCListItem);
    QQuickItem *grabber = window ? window->mouseGrabberItem() : Q_NULLPTR;
    if (grabber == q) {
        // already the grabber, return
        return true;
    }

    bool consumed = false;
    if (contentItem->contains(contentItem->mapFromScene(event->windowPos()))) {
        QPointF localPos = q->mapFromScene(event->windowPos());

        switch (event->type()) {
        case QEvent::MouseButtonPress: {
            // remember pressed point over active areas
            if (event->button() == Qt::LeftButton) {
                if (swiped) {
                    // handle as full press and grab the event from the children
                    QScopedPointer<QMouseEvent> mouseEvent(QQuickWindowPrivate::cloneMouseEvent(event, &localPos));
                    handleLeftButtonPress(mouseEvent.data());
                    consumed = true;
                } else {
                    // remember the position
                    pressedPos = localPos;
                    button = event->button();
                }
            }
            break;
        }
        case QEvent::MouseButtonRelease: {
            QScopedPointer<QMouseEvent> mouseEvent(QQuickWindowPrivate::cloneMouseEvent(event, &localPos));
            handleLeftButtonRelease(mouseEvent.data());
            suppressClick = false;
            break;
        }
        case QEvent::MouseMove: {
            if ((button == Qt::LeftButton) && swipedOverThreshold(localPos, pressedPos) && !highlighted) {
                // grab the event from the child, so the click doesn't happen anymore, and initiate swiping
                QMouseEvent pressed(QEvent::MouseButtonPress, localPos, event->windowPos(), event->screenPos(),
                                        Qt::LeftButton, event->buttons(), event->modifiers());
                handleLeftButtonPress(&pressed);
                // grab any further events so all land in the list item
                q->setKeepMouseGrab(true);
                q->grabMouse();
                consumed = true;
            }
            break;
        }
        default: break;
        }
    }

    return consumed;
}

bool UCListItem::childMouseEventFilter(QQuickItem *child, QEvent *event)
{
    if (!isVisible() || !isEnabled()) {
        return UCStyledItemBase::childMouseEventFilter(child, event);
    }

    Q_D(UCListItem);
    switch (event->type()) {
    case QEvent::MouseButtonPress:
    case QEvent::MouseMove:
    case QEvent::MouseButtonRelease: {
        if (d->sendMouseEvent(child, static_cast<QMouseEvent*>(event))) {
            return true;
        }
    }
    default: break;
    }

    return UCStyledItemBase::childMouseEventFilter(child, event);
}

bool UCListItem::eventFilter(QObject *target, QEvent *event)
{
    QPointF myPos;
    // only filter press events, and rebound when pressed outside
    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        QQuickWindow *window = qobject_cast<QQuickWindow*>(target);
        if (window) {
            myPos = window->contentItem()->mapToItem(this, mouse->localPos());
        }
    } else if (event->type() == QEvent::TouchBegin) {
        QTouchEvent *touch = static_cast<QTouchEvent*>(event);
        QQuickWindow *window = qobject_cast<QQuickWindow*>(target);
        if (window) {
            myPos = window->contentItem()->mapToItem(this, touch->touchPoints()[0].pos());
        }
    }
    if (!myPos.isNull() && !contains(myPos)) {
        Q_D(UCListItem);
        d->snapOut();
        // only accept event, but let it be handled by the underlying or surrounding Flickables
        event->accept();
    }
    return UCStyledItemBase::eventFilter(target, event);
}

void UCListItem::timerEvent(QTimerEvent *event)
{
    Q_D(UCListItem);
    if (event->timerId() == d->pressAndHoldTimer.timerId() && d->highlighted && !d->swiped) {
        d->pressAndHoldTimer.stop();
        if (isEnabled() && d->isPressAndHoldConnected()) {
            d->suppressClick = true;
            Q_EMIT pressAndHold();
        }
    } else {
        QQuickItem::timerEvent(event);
    }
}

/*!
 * \qmlproperty ListItemActions ListItem::leadingActions
 *
 * The property holds the actions and its configuration to be revealed when swiped
 * from left to right.
 *
 * \sa trailingActions
 */
UCListItemActions *UCListItem::leadingActions() const
{
    Q_D(const UCListItem);
    return d->leadingActions;
}
void UCListItem::setLeadingActions(UCListItemActions *actions)
{
    Q_D(UCListItem);
    if (d->leadingActions == actions) {
        return;
    }
    // snap out before we change the actions
    d->snapOut();
    d->leadingActions = actions;
    Q_EMIT leadingActionsChanged();
}

/*!
 * \qmlproperty ListItemActions ListItem::trailingActions
 *
 * The property holds the actions and its configuration to be revealed when swiped
 * from right to left.
 *
 * \sa leadingActions
 */
UCListItemActions *UCListItem::trailingActions() const
{
    Q_D(const UCListItem);
    return d->trailingActions;
}
void UCListItem::setTrailingActions(UCListItemActions *actions)
{
    Q_D(UCListItem);
    if (d->trailingActions == actions) {
        return;
    }
    // snap out before we change the actions
    d->snapOut();
    d->trailingActions = actions;
    Q_EMIT trailingActionsChanged();
}

/*!
 * \qmlproperty Item ListItem::contentItem
 *
 * contentItem holds the components placed on a ListItem. It is anchored to the
 * ListItem on left, top and right, and to the divider on the bottom, or to the
 * ListItem's bottom in case the divider is not visible. The content is clipped
 * by default. It is not recommended to change the anchors as the ListItem controls
 * them, however any other property value is free to change.
 * Example:
 * \qml
 * ListItem {
 *     contentItem.anchors {
 *         leftMargin: units.gu(2)
 *         rightMargin: units.gu(2)
 *         topMargin: units.gu(0.5)
 *         bottomMargin: units.gu(0.5)
 *     }
 * }
 * \endqml
 */
QQuickItem* UCListItem::contentItem() const
{
    Q_D(const UCListItem);
    return d->contentItem;
}

/*!
 * \qmlpropertygroup ::ListItem::divider
 * \qmlproperty real ListItem::divider.colorFrom
 * \qmlproperty real ListItem::divider.colorTo
 *
 * This grouped property configures the thin divider shown in the bottom of the
 * component. The divider is not moved together with the content when swiped left
 * or right to reveal the actions. \b colorFrom and \b colorTo configure
 * the starting and ending colors of the divider. Beside these properties all Item
 * specific properties can be accessed.
 *
 * When \b visible is true, the ListItem's content size gets thinner with the
 * divider's \b thickness. By default the divider is anchored to the bottom, left
 * right of the ListItem, and has a 2dp height.
 */
UCListItemDivider* UCListItem::divider() const
{
    Q_D(const UCListItem);
    return d->divider;
}

/*!
 * \qmlproperty bool ListItem::highlighted
 * True when the item is pressed. The items stays highlighted when the mouse or touch
 * is moved horizontally. When in Flickable (or ListView), the item gets un-highlighted
 * (false) when the mouse or touch is moved towards the vertical direction causing
 * the flickable to move.
 *
 * Configures the color when highlighted. Defaults to the theme palette's background
 * color.
 *
 * An item is highlighted, thus highlight state toggled, when pressed and it has
 * one of the following conditions fulfilled:
 * \list
 *  \li * \l leadingActions or \l trailingActions set,
 *  \li * it has an \l action attached
 *  \li * if the ListItem has an active child component, such as a \l Button, a
 *      \l Switch, etc.
 *  \li * in general, if an active (enabled and visible) \b MouseArea is added
 *      as a child component
 *  \li * \l clicked signal handler is implemented or there is a slot or function
 *      connected to it
 *  \li * \l pressAndHold signal handler is implemented or there is a slot or
 *      function connected to it.
 * \endlist
 *
 * \note Adding an active component does not mean the component will be activated
 * when the ListItem will be tapped/clicked outside of the component area. If
 * such a behavior is needed, that must be done explicitly.
 * \qml
 * ListItem {
 *     Label {
 *         text: "This is a label"
 *     }
 *     Switch {
 *         id: toggle
 *         anchors.right: parent.right
 *     }
 *     Component.onCompleted: clicked.connect(toggle.clicked)
 * }
 * \endqml
 *
 * \sa action, leadingActions, trailingActions
 */
bool UCListItem::highlighted() const
{
    Q_D(const UCListItem);
    return d->highlighted;
}

/*!
 * \qmlproperty bool ListItem::contentMoving
 * \readonly
 * The property describes whether the content is moving or not. The content is
 * moved when swiped or when snapping in or out, and lasts till the snapping
 * animation completes.
 */

/*!
 * \qmlsignal ListItem::contentMovementStarted()
 * The signal is emitted when the content movement has started.
 */

/*!
 * \qmlsignal ListItem::contentMovementEnded()
 * The signal is emitted when the content movement has ended.
 */
bool UCListItemPrivate::contentMoving() const
{
    return contentMoved;
}
void UCListItemPrivate::setContentMoving(bool moved)
{
    if (contentMoved == moved) {
        return;
    }
    contentMoved = moved;
    Q_Q(UCListItem);
    if (contentMoved) {
        Q_EMIT q->contentMovementStarted();
    } else {
        Q_EMIT q->contentMovementEnded();
    }
    Q_EMIT q->contentMovingChanged();
}

/*!
 * \qmlproperty color ListItem::color
 * Configures the color of the normal background. The default value is transparent.
 */
QColor UCListItem::color() const
{
    Q_D(const UCListItem);
    return d->color;
}
void UCListItem::setColor(const QColor &color)
{
    Q_D(UCListItem);
    if (d->color == color) {
        return;
    }
    d->color = color;
    update();
    Q_EMIT colorChanged();
}

/*!
 * \qmlproperty color ListItem::highlightColor
 * Configures the color when highlighted. Defaults to the theme palette's background
 * color. If changed, it can be reset by assigning undefined as value.
 */
QColor UCListItem::highlightColor() const
{
    Q_D(const UCListItem);
    return d->highlightColor;
}
void UCListItem::setHighlightColor(const QColor &color)
{
    Q_D(UCListItem);
    // mark it as custom even if the value is the same as the previous one
    d->customColor = true;
    if (d->highlightColor == color) {
        return;
    }
    d->highlightColor = color;
    update();
    Q_EMIT highlightColorChanged();
}
void UCListItem::resetHighlightColor()
{
    Q_D(UCListItem);
    d->customColor = false;
    d->highlightColor = getTheme()->getPaletteColor("selected", "background");
    update();
    Q_EMIT highlightColorChanged();
}

/*!
 * \qmlproperty bool ListItem::dragging
 * The property informs about an ongoing dragging on a ListItem.
 */
bool UCListItemPrivate::dragging()
{
    return !dragHandler.isNull();
}

/*!
 * \qmlproperty bool ListItem::dragMode
 * The property reports whether a ListItem is draggable or not. While in drag mode,
 * the list item content cannot be swiped. The default value is false.
 */
bool UCListItemPrivate::dragMode()
{
    UCViewItemsAttachedPrivate *attached = UCViewItemsAttachedPrivate::get(parentAttached);
    return attached ? attached->draggable : false;
}
void UCListItemPrivate::setDragMode(bool draggable)
{
    if (parentAttached) {
        parentAttached->setDragMode(draggable);
    }
}

/*!
 * 
 * \qmlproperty bool ListItem::selected
 * The property drives whether a list item is selected or not. Defaults to false.
 *
 * \sa ListItem::selectMode, ViewItems::selectMode
 */
bool UCListItemPrivate::isSelected()
{
    Q_Q(UCListItem);
    return UCViewItemsAttachedPrivate::get(parentAttached)->isItemSelected(q);
}
void UCListItemPrivate::setSelected(bool value)
{
    Q_Q(UCListItem);
    if (value) {
        UCViewItemsAttachedPrivate::get(parentAttached)->addSelectedItem(q);
    } else {
        UCViewItemsAttachedPrivate::get(parentAttached)->removeSelectedItem(q);
    }
}

/*!
 * \qmlproperty bool ListItem::selectMode
 * The property reports whether the component and the view using the component
 * is in selectable state. While selectable, the ListItem's leading- and trailing
 * panels cannot be swiped in. \l clicked and \l pressAndHold signals are also
 * triggered. Selectable mode can be set either through this property or through
 * the parent attached \l ViewItems::selectMode property.
 */
bool UCListItemPrivate::selectMode()
{
    UCViewItemsAttachedPrivate *attached = UCViewItemsAttachedPrivate::get(parentAttached);
    return attached ? attached->selectable : false;
}
void UCListItemPrivate::setSelectMode(bool selectable)
{
    if (parentAttached) {
        parentAttached->setSelectMode(selectable);
    }
}

/*!
 * \qmlproperty Action ListItem::action
 * The property holds the action which will be triggered when the ListItem is
 * clicked. ListItem will not visualize the action, that is the responsibility
 * of the components placed inside the list item. However, when set, the ListItem
 * will be highlighted on press.
 *
 * If the action set has no value type set, ListItem will set its type to \b
 * Action.Integer and the \l {Action::triggered}{triggered} signal will be getting
 * the ListItem index as \e value parameter.
 *
 * Defaults no null.
 */
UCAction *UCListItemPrivate::action() const
{
    return mainAction;
}
void UCListItemPrivate::setAction(UCAction *action)
{
    Q_Q(UCListItem);
    if (mainAction == action) {
        return;
    }
    mainAction = action;
    if (mainAction && (mainAction->m_parameterType == UCAction::None)) {
        // call setProperty to invoke notify signal
        mainAction->setProperty("parameterType", UCAction::Integer);
    }
    Q_EMIT q->actionChanged();
}

/*!
 * \qmlproperty list<Object> ListItem::listItemData
 * \default
 * \internal
 * Overloaded default property containing all the children and resources.
 */
QQmlListProperty<QObject> UCListItemPrivate::data()
{
    return QQuickItemPrivate::get(contentItem)->data();
}

/*!
 * \qmlproperty list<Item> ListItem::listItemChildren
 * \internal
 * Overloaded default property containing all the visible children of the item.
 */
QQmlListProperty<QQuickItem> UCListItemPrivate::children()
{
    return QQuickItemPrivate::get(contentItem)->children();
}

/*!
 * \qmlpropertygroup ::ListItem::expansion
 * \qmlproperty bool ListItem::expansion.expanded
 * \qmlproperty real ListItem::expansion.height
 * \since Ubuntu.Components 1.3
 * The group drefines the expansion state of the ListItem.
 */
UCListItemExpansion *UCListItem::expansion()
{
    Q_D(UCListItem);
    if (!d->expansion) {
        d->expansion = new UCListItemExpansion(this);
    }
    return d->expansion;
}

void UCListItemPrivate::_q_updateExpansion(const QList<int> &indices)
{
    Q_Q(UCListItem);
    Q_EMIT q->expansion()->expandedChanged();
    // make sure the style is loaded
    if (indices.contains(index())) {
        loadStyleItem();
    }
}

#include "moc_uclistitem.cpp"
