/*
 * Copyright 2014 Canonical Ltd.
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
#include "uclistitem.h"
#include "uclistitem_p.h"
#include "uclistitemactions.h"
#include "uclistitemactions_p.h"
#include "ucubuntuanimation.h"
#include "propertychange_p.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>
#include <QtQuick/private/qquickanimation_p.h>
#include "ucubuntuanimation.h"

#define MIN(x, y)           ((x < y) ? x : y)
#define MAX(x, y)           ((x > y) ? x : y)
#define CLAMP(v, min, max)  (min <= max) ? MAX(min, MIN(v, max)) : MAX(max, MIN(v, min))

QColor getPaletteColor(const char *profile, const char *color)
{
    QColor result;
    QObject *palette = UCTheme::instance().palette();
    if (palette) {
        QObject *paletteProfile = palette->property(profile).value<QObject*>();
        if (paletteProfile) {
            result = paletteProfile->property(color).value<QColor>();
        }
    }
    return result;
}
/******************************************************************************
 * SnapAnimator
 */
UCListItemSnapAnimator::UCListItemSnapAnimator(UCListItem *item)
    : QObject(item)
    , item(item)
    , defaultAnimation(0)
{
}
UCListItemSnapAnimator::~UCListItemSnapAnimator()
{
    // make sure we cannot animate anymore, for safety
    item = 0;
    qDebug() << "DESTROY ANIMATOR";
}

void UCListItemSnapAnimator::setCustomAnimation(QQuickPropertyAnimation *animation)
{
    if (animation) {
        // delete default animation so we use this
        delete defaultAnimation;
        defaultAnimation = 0;
    }
}

bool UCListItemSnapAnimator::snap(qreal to)
{
    if (!item) {
        return false;
    }
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    QQuickPropertyAnimation *snap = listItem->snap ? listItem->snap : getDefaultAnimation();
    if (!snap) {
        return false;
    }
    snap->setTargetObject(listItem->contentItem);
    if (to == 0.0) {
        QObject::connect(snap, &QQuickAbstractAnimation::stopped,
                         this, &UCListItemSnapAnimator::snapOut);
    } else {
        QObject::connect(snap, &QQuickAbstractAnimation::stopped,
                         this, &UCListItemSnapAnimator::snapIn);
    }
    if (snap->properties().isEmpty() && snap->property().isEmpty()) {
        snap->setProperty("x");
    }
    snap->setFrom(listItem->contentItem->property(snap->property().toLocal8Bit().constData()));
    snap->setTo(to);
    snap->setAlwaysRunToEnd(true);
    listItem->setContentMoved(true);
    snap->start();
    return true;
}

void UCListItemSnapAnimator::snapOut()
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    QQuickAbstractAnimation *snap = listItem->snap ? listItem->snap : getDefaultAnimation();
    // disconnect animation, otherwise snapping will disconnect the panel
    QObject::disconnect(snap, 0, 0, 0);
    // restore flickable's interactive and cleanup
    listItem->attachedProperties->disableInteractive(item, false);
    // no need to listen flickables any longer
    listItem->attachedProperties->listenToRebind(item, false);
    // disconnect actions
    listItem->grabPanel(listItem->leadingActions, false);
    listItem->grabPanel(listItem->trailingActions, false);
    // set contentMoved to false
    listItem->setContentMoved(false);
}

void UCListItemSnapAnimator::snapIn()
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    QQuickAbstractAnimation *snap = listItem->snap ? listItem->snap : getDefaultAnimation();
    QObject::disconnect(snap, 0, 0, 0);
    listItem->setContentMoved(false);
}

QQuickPropertyAnimation *UCListItemSnapAnimator::getDefaultAnimation()
{
    if (defaultAnimation) {
        return defaultAnimation;
    }

    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    // create rebound animation
    UCUbuntuAnimation animationCodes;
    defaultAnimation = new QQuickPropertyAnimation(this);
    QEasingCurve easing(QEasingCurve::OutElastic);
    easing.setPeriod(0.5);
    defaultAnimation->setEasing(easing);
    defaultAnimation->setDuration(animationCodes.BriskDuration());
    defaultAnimation->setTargetObject(listItem->contentItem);
    defaultAnimation->setProperty("x");
    defaultAnimation->setAlwaysRunToEnd(true);
    return defaultAnimation;
}

/******************************************************************************
 * Divider
 */
UCListItemDivider::UCListItemDivider(QObject *parent)
    : QObject(parent)
    , m_visible(true)
    , m_leftMarginChanged(false)
    , m_rightMarginChanged(false)
    , m_colorFromChanged(false)
    , m_colorToChanged(false)
    , m_thickness(0)
    , m_leftMargin(0)
    , m_rightMargin(0)
    , m_listItem(0)
{
    connect(&UCUnits::instance(), &UCUnits::gridUnitChanged, this, &UCListItemDivider::unitsChanged);
    connect(&UCTheme::instance(), &UCTheme::paletteChanged, this, &UCListItemDivider::paletteChanged);
    unitsChanged();
    paletteChanged();
}
UCListItemDivider::~UCListItemDivider()
{
}

void UCListItemDivider::init(UCListItem *listItem)
{
    QQml_setParent_noEvent(this, listItem);
    m_listItem = UCListItemPrivate::get(listItem);
}

void UCListItemDivider::unitsChanged()
{
    m_thickness = UCUnits::instance().dp(2);
    if (!m_leftMarginChanged) {
        m_leftMargin = UCUnits::instance().dp(2);
    }
    if (!m_rightMarginChanged) {
        m_rightMargin = UCUnits::instance().dp(2);
    }
    if (m_listItem) {
        m_listItem->update();
    }
}

void UCListItemDivider::paletteChanged()
{
    QColor background = getPaletteColor("normal", "background");
    if (!background.isValid()) {
        return;
    }
    // FIXME: we need a palette value for divider colors, till then base on the background
    // luminance
    if (!m_colorFromChanged || !m_colorToChanged) {
        qreal luminance = (background.red()*212 + background.green()*715 + background.blue()*73)/1000.0/255.0;
        bool lightBackground = (luminance > 0.85);
        if (!m_colorFromChanged) {
            m_colorFrom = lightBackground ? QColor("#26000000") : QColor("#26FFFFFF");
        }
        if (!m_colorToChanged) {
            m_colorTo = lightBackground ? QColor("#14FFFFFF") : QColor("#14000000");
        }
        updateGradient();
    }
}

void UCListItemDivider::updateGradient()
{
    m_gradient.clear();
    m_gradient.append(QGradientStop(0.0, m_colorFrom));
    m_gradient.append(QGradientStop(0.49, m_colorFrom));
    m_gradient.append(QGradientStop(0.5, m_colorTo));
    m_gradient.append(QGradientStop(1.0, m_colorTo));
    if (m_listItem) {
        m_listItem->update();
    }
}

QSGNode *UCListItemDivider::paint(QSGNode *node, const QRectF &rect)
{
    QSGRectangleNode *dividerNode = static_cast<QSGRectangleNode*>(node);
    if (m_visible && (m_gradient.size() > 0) && ((m_colorFrom.alphaF() >= (1.0f / 255.0f)) || (m_colorTo.alphaF() >= (1.0f / 255.0f)))) {
        if (!dividerNode) {
            dividerNode = m_listItem->sceneGraphContext()->createRectangleNode();
        }
        QRectF divider(m_leftMargin, rect.height() - m_thickness, rect.width() - m_leftMargin - m_rightMargin, m_thickness);
        dividerNode->setRect(divider);
        dividerNode->setGradientStops(m_gradient);
        dividerNode->update();
        return dividerNode;
    } else if (node) {
        // delete the node
        delete node;
    }
    return 0;
}

void UCListItemDivider::setVisible(bool visible)
{
    if (m_visible == visible) {
        return;
    }
    m_visible = visible;
    m_listItem->resize();
    m_listItem->update();
    Q_EMIT visibleChanged();
}

void UCListItemDivider::setLeftMargin(qreal leftMargin)
{
    if (m_leftMargin == leftMargin) {
        return;
    }
    m_leftMargin = leftMargin;
    m_leftMarginChanged = true;
    m_listItem->update();
    Q_EMIT leftMarginChanged();
}

void UCListItemDivider::setRightMargin(qreal rightMargin)
{
    if (m_rightMargin == rightMargin) {
        return;
    }
    m_rightMargin = rightMargin;
    m_rightMarginChanged = true;
    m_listItem->update();
    Q_EMIT rightMarginChanged();
}

void UCListItemDivider::setColorFrom(const QColor &color)
{
    if (m_colorFrom == color) {
        return;
    }
    m_colorFrom = color;
    m_colorFromChanged = true;
    updateGradient();
    Q_EMIT colorFromChanged();
}

void UCListItemDivider::setColorTo(const QColor &color)
{
    if (m_colorTo == color) {
        return;
    }
    m_colorTo = color;
    m_colorToChanged = true;
    updateGradient();
    Q_EMIT colorToChanged();
}

/******************************************************************************
 * ListItemPrivate
 */
UCListItemPrivate::UCListItemPrivate()
    : UCStyledItemBasePrivate()
    , pressed(false)
    , highlightColorChanged(false)
    , tugged(false)
    , suppressClick(false)
    , contentMoving(false)
    , ready(false)
    , customPanel(false)
    , customColor(false)
    , xAxisMoveThresholdGU(1.5)
    , overshootGU(2)
    , color(Qt::transparent)
    , highlightColor(Qt::transparent)
    , attachedProperties(0)
    , contentItem(new QQuickItem)
    , divider(new UCListItemDivider)
    , leadingActions(0)
    , trailingActions(0)
    , snap(0)
    , animator(0)
    , actionsPanel(0)
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
    divider->init(q);
    // content will be redirected to the contentItem, therefore we must report
    // children changes as it would come from the main component
    QObject::connect(contentItem, &QQuickItem::childrenChanged,
                     q, &UCListItem::childrenChanged);
    q->setFlag(QQuickItem::ItemHasContents);
    // turn activeFocusOnPress on
    q->setActiveFocusOnPress(true);

    // catch theme changes
    QObject::connect(&UCTheme::instance(), SIGNAL(nameChanged()), q, SLOT(_q_updateThemedData()));
    _q_updateThemedData();

    // watch size change and set implicit size;
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateSize()));
    _q_updateSize();
}

void UCListItemPrivate::_q_updateThemedData()
{
    // update colors, panels
    if (!customColor) {
        Q_Q(UCListItem);
        highlightColor = getPaletteColor("selected", "background");
        q->update();
    }
    updateActionsPanel();
}

void UCListItemPrivate::_q_rebound()
{
    setPressed(false);
    // initiate rebinding only if there were actions tugged
    Q_Q(UCListItem);
    if (!UCListItemActionsPrivate::isConnectedTo(leadingActions, q) && !UCListItemActionsPrivate::isConnectedTo(trailingActions, q)) {
        return;
    }
    setTugged(false);
    // rebound to zero
    animator->snap(0);
}
/*!
 * \qmlproperty bool ListItem::moving
 * The property signals the move of the list item's content. It is set whenever
 * the content is tugged and reset when the snapping and rebounding animations
 * complete.
 *
 * \sa movingStarted, movingEnded
 */

/*!
 * \qmlsignal ListItem::movingStarted
 * Signal emitted when the moving of the list item content is started.
 */
/*!
 * \qmlsignal ListItem::movingEnded
 * Signal emitted when the moving of the list item content is ended.
 */
bool UCListItemPrivate::isMoving() const
{
    return contentMoving;
}
// the function drives the moving property
void UCListItemPrivate::setContentMoved(bool move)
{
    if (contentMoving == move) {
        return;
    }
    contentMoving = move;
    Q_Q(UCListItem);
    if (move) {
        Q_EMIT q->movementStarted();
    } else {
        Q_EMIT q->movementEnded();
    }
    Q_EMIT q->movingChanged();
}

/*!
 * \qmlproperty Component ListItem::actionsDelegate
 * The property configures the component visualizing and triggering the actions is
 * created from. The default delegate is taken from the current theme.
 *
 * Custom delegates are responsible to visualize and handle snapping and triggering
 * of the actions listed in a given action list. The same delegate must handle both
 * leading and trailing actions at the same time. The actions amongst other action
 * list related properties can be accessed through the \l ListItemActions attached
 * properties.
 *
 * The following example illustrates how a custom panel can be implemented. The
 * actions are triggered each time the tug is stopped. There will be only one
 * action shown depending how far the content is tugged, and the color of the
 * panel is changing depending on which action is shown.
 * \qml
 * import QtQuick 2.2
 * import Ubuntu.Components 1.2
 *
 * UbuntuListView {
 *     model: 100
 *     delegate: ListItem {
 *         StandardLayout {
 *             captions.title.text: "Caption (title text)"
 *             details {
 *                 title.text: "Text"
 *                 subtitle.text: "Text"
 *             }
 *         }
 *         trailingActions: trailing
 *         actionsDelegate: Rectangle {
 *             id: panel
 *             property bool leadingPanel: ListItemActions.status == ListItemActions.Leading
 *             property Item contentItem: (ListItemActions.container && ListItemActions.container.connectedItem) ?
 *                                            ListItemActions.container.connectedItem.contentItem : null
 *             anchors {
 *                 left: contentItem ? contentItem.right : undefined
 *                 top: contentItem ? contentItem.top : undefined
 *                 bottom: contentItem ? contentItem.bottom : undefined
 *             }
 *             width: contentItem ? (contentItem.width - units.gu(10)) : 0
 *             color: colors[visibleAction]
 *
 *             property real slotSize: panel.width / ListItemActions.container.actions.length
 *             // give a small margin so we don't jump to the next item
 *             property int visibleAction: (slotSize > 0) ?
 *                      MathUtils.clamp((ListItemActions.offsetVisible - 1) / slotSize, 0, 3) : 0
 *             property var colors: [UbuntuColors.blue, UbuntuColors.lightGrey, UbuntuColors.coolGrey]
 *
 *             Item {
 *                 anchors {
 *                     left: parent.left
 *                     top: parent.top
 *                     bottom: parent.bottom
 *                 }
 *                 width: height
 *                 Icon {
 *                     width: units.gu(3)
 *                     height: width
 *                     anchors.centerIn: parent
 *                     color: "white"
 *                     name: panel.ListItemActions.container.actions[visibleAction].iconName
 *                 }
 *             }
 *
 *             ListItemActions.onDraggingChanged: {
 *                 if (!ListItemActions.dragging) {
 *                     // snap first, then trigger
 *                     ListItemActions.snapToPosition((visibleAction + 1) * slotSize);
 *                     panel.ListItemActions.container.actions[visibleAction].triggered(panel.ListItemActions.itemIndex)
 *                 }
 *             }
 *         }
 *     }
 *     ListItemActions {
 *         id: trailing
 *         actions: [
 *             Action {
 *                 iconName: "alarm-clock"
 *                 onTriggered: print(iconName, "triggered", value)
 *             },
 *             Action {
 *                 iconName: "camcorder"
 *                 onTriggered: print(iconName, "triggered", value)
 *             },
 *             Action {
 *                 iconName: "stock_website"
 *                 onTriggered: print(iconName, "triggered", value)
 *             }
 *         ]
 *     }
 * }
 * \endqml
 */
QQmlComponent *UCListItemPrivate::actionsDelegate() const
{
    return actionsPanel;
}
void UCListItemPrivate::setActionsDelegate(QQmlComponent *delegate)
{
    if (actionsPanel == delegate) {
        return;
    }
    // make sure we're rebound before we change the panel component
    promptRebound();
    if (!customPanel) {
        // delete theme created delegate
        delete actionsPanel;
        actionsPanel = 0;
    }
    actionsPanel = delegate;
    customPanel = (actionsPanel == 0);
    Q_Q(UCListItem);
    if (ready && !customPanel) {
        // need to get the default one from the theme
        actionsPanel = UCTheme::instance().createStyleComponent("ListItemPanel.qml", q);
    }
    Q_EMIT q->actionsDelegateChanged();
}

// update themed components
void UCListItemPrivate::updateActionsPanel()
{
    if (!ready) {
        return;
    }
    if (!customPanel) {
        delete actionsPanel;
        Q_Q(UCListItem);
        actionsPanel = UCTheme::instance().createStyleComponent("ListItemPanel.qml", q);
    }
}

// the function performs a prompt rebound on mouse release without any animation
void UCListItemPrivate::promptRebound()
{
    setPressed(false);
    setTugged(false);
    if (animator) {
        animator->snapOut();
    }
}

// called when units size changes
void UCListItemPrivate::_q_updateSize()
{
    Q_Q(UCListItem);
    QQuickItem *owner = flickable ? flickable : parentItem;
    q->setImplicitWidth(owner ? owner->width() : UCUnits::instance().gu(40));
    q->setImplicitHeight(UCUnits::instance().gu(7));
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

// set pressed flag and update contentItem
void UCListItemPrivate::setPressed(bool pressed)
{
    if (this->pressed != pressed) {
        this->pressed = pressed;
        suppressClick = false;
        Q_Q(UCListItem);
        q->update();
        Q_EMIT q->pressedChanged();
    }
}
// toggles the tugged flag and installs/removes event filter
void UCListItemPrivate::setTugged(bool tugged)
{
    suppressClick = tugged;
    if (this->tugged == tugged) {
        return;
    }
    this->tugged = tugged;
    Q_Q(UCListItem);
    QQuickWindow *window = q->window();
    if (tugged) {
        window->installEventFilter(q);
    } else {
        window->removeEventFilter(q);
    }
}

// sets the tugged flag but also grabs the panels from the leading/trailing actions
bool UCListItemPrivate::grabPanel(UCListItemActions *actionsList, bool isTugged)
{
    Q_Q(UCListItem);
    if (isTugged) {
        bool grab = UCListItemActionsPrivate::connectToListItem(actionsList, q, (actionsList == leadingActions));
        if (attachedProperties) {
            attachedProperties->disableInteractive(q, true);
        }
        return grab;
    } else {
        UCListItemActionsPrivate::disconnectFromListItem(actionsList);
        return false;
    }
}


// connects/disconnects from the Flickable anchestor to get notified when to do rebound
void UCListItemPrivate::listenToRebind(bool listen)
{
    if (attachedProperties) {
        Q_Q(UCListItem);
        attachedProperties->listenToRebind(q, listen);
    }
}

void UCListItemPrivate::resize()
{
    Q_Q(UCListItem);
    QRectF rect(q->boundingRect());
    if (divider && divider->m_visible) {
        rect.setHeight(rect.height() - divider->m_thickness);
    }
    contentItem->setSize(rect.size());
}

void UCListItemPrivate::update()
{
    if (!ready) {
        return;
    }
    Q_Q(UCListItem);
    q->update();
}

void UCListItemPrivate::clampX(qreal &x, qreal dx)
{
    UCListItemActionsPrivate *leading = UCListItemActionsPrivate::get(leadingActions);
    UCListItemActionsPrivate *trailing = UCListItemActionsPrivate::get(trailingActions);
    x += dx;
    // min cannot be less than the trailing's panel width
    qreal min = (trailing && trailing->panelItem) ? -trailing->panelItem->width() : 0;
    // max cannot be bigger than 0 or the leading's width in case we have leading panel
    qreal max = (leading && leading->panelItem) ? leading->panelItem->width() : 0;
    x = CLAMP(x, min, max);
}

/*!
 * \qmltype ListItem
 * \instantiates UCListItem
 * \inqmlmodule Ubuntu.Components 1.2
 * \ingroup unstable-ubuntu-listitems
 * \since Ubuntu.Components 1.2
 * \brief The ListItem element provides Ubuntu design standards for list or grid
 * views.
 *
 * The component is dedicated to be used in designs with static or dynamic lists
 * (i.e. list views where each item's layout differs or in lists where the content
 * is determined by a given model, thus each element has the same layout). The
 * element does not define any specific layout, components can be placed in any
 * ways on it. However, when used in list views, the content must be carefully
 * chosen to in order to keep the kinetic behavior and the highest FPS possible.
 *
 * The component provides two color properties which configures the item's background
 * when normal or pressed. This can be configures through \l color and \l highlightColor
 * properties.
 *
 * \c contentItem holds all components and resources declared as child to ListItem.
 * Being an Item, all other properties can be accessed or altered, with the exception
 * of some:
 * \list A
 * \li do not alter \c x, \c y, \c width or \c height properties as those are
 *      controlled by the ListItem itself when leading or trailing actions are
 *      revealed and thus will destroy your logic
 * \li never anchor left or right anchor lines as it will block revealing the actions.
 * \endlist
 *
 * Each ListItem has a thin divider shown on the bottom of the component. This
 * divider can be configured through the \l divider grouped property, which can
 * configure its margins from the edges of the ListItem as well as its visibility.
 *
 * ListItem can handle actions that can get tugged from front to back of the item.
 * These actions are Action elements visualized in panels attached to the front
 * or to the back of the item, and are revealed by swiping the item horizontally.
 * The tug is started only after the mouse/touch move had passed a given threshold.
 * These actions are configured through the \l leadingActions as well as \l
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
 * \note When a list item is tugged, it automatically connects both leading and
 * trailing actions to the list item. This implies that a ListItem cannot use
 * the same ListItemActions instance for both leading and trailing actions. If
 * it is desired to have the same action present in both leading and trailing
 * actions, one of the ListItemActions actions list can use the other's list. In
 * the following example the list item can be deleted through both leading and
 * trailing actions:
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
 *         actions: leadingActions.actions
 *     }
 * }
 * \endqml
 * \sa ListItemActions
 */

/*!
 * \qmlsignal ListItem::clicked()
 *
 * The signal is emitted when the component gets released while the \l pressed property
 * is set. The signal is not emitted if the ListItem content is tugged or when used in
 * Flickable (or ListView, GridView) and the Flickable gets moved.
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

UCListItemAttached *UCListItem::qmlAttachedProperties(QObject *owner)
{
    return new UCListItemAttached(owner);
}

void UCListItem::componentComplete()
{
    UCStyledItemBase::componentComplete();
    d_func()->ready = true;
}

void UCListItem::itemChange(ItemChange change, const ItemChangeData &data)
{
    UCStyledItemBase::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        Q_D(UCListItem);
        // make sure we are not connected to the previous Flickable
        d->listenToRebind(false);
        // check if we are in a positioner, and if that positioner is in a Flickable
        QQuickBasePositioner *positioner = qobject_cast<QQuickBasePositioner*>(data.item);
        if (positioner && positioner->parentItem()) {
            d->flickable = qobject_cast<QQuickFlickable*>(positioner->parentItem()->parentItem());
        } else if (data.item && data.item->parentItem()){
            // check if we are in a Flickable then
            d->flickable = qobject_cast<QQuickFlickable*>(data.item->parentItem());
        }

        // attach ListItem properties to the flickable or to the parent item
        if (d->flickable) {
            // connect to flickable to get width changes
            d->attachedProperties = static_cast<UCListItemAttached*>(qmlAttachedPropertiesObject<UCListItem>(d->flickable));
        } else if (data.item) {
            d->attachedProperties = static_cast<UCListItemAttached*>(qmlAttachedPropertiesObject<UCListItem>(data.item));
        } else {
            // about to be deleted or reparrented, disable attached
            d->attachedProperties = 0;
        }

        if (data.item) {
            QObject::connect(d->flickable ? d->flickable : data.item, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
        }

        // update size
        d->_q_updateSize();
    }
}

void UCListItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    UCStyledItemBase::geometryChanged(newGeometry, oldGeometry);
    // resize contentItem item
    Q_D(UCListItem);
    d->resize();
}

QSGNode *UCListItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    Q_D(UCListItem);
    QColor color = d->pressed ? d->highlightColor : d->color;

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
        // cover only the area of the contentItem
        rectNode->setRect(d->contentItem->boundingRect());
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
    QSGNode *dividerNode = oldNode ? oldNode->childAtIndex(0) : 0;
    if (d->divider && d->divider->m_visible) {
        QSGNode *newNode = d->divider->paint(dividerNode, boundingRect());
        if (newNode != dividerNode && oldNode) {
            if (dividerNode) {
                oldNode->removeChildNode(dividerNode);
            }
            if (newNode) {
                oldNode->appendChildNode(newNode);
            }
        }
        if (!oldNode) {
            oldNode = newNode;
        }
    } else if (dividerNode) {
        // the divider painter node may be still added as child, so remove it
        oldNode->removeChildNode(dividerNode);
    }
    return oldNode;
}

void UCListItem::mousePressEvent(QMouseEvent *event)
{
    UCStyledItemBase::mousePressEvent(event);
    Q_D(UCListItem);
    if (d->attachedProperties && d->attachedProperties->isMoving()) {
        // while moving, we cannot select any items
        return;
    }
    if (event->button() == Qt::LeftButton) {
        d->setPressed(true);
        d->lastPos = d->pressedPos = event->localPos();
        // connect the Flickable to know when to rebound
        d->listenToRebind(true);
        // if it was moved, grab the panels
        if (d->tugged) {
            d->grabPanel(d->leadingActions, true);
            d->grabPanel(d->trailingActions, true);
        }
    }
    // accept the event so we get the rest of the events as well
    event->setAccepted(true);
}

void UCListItem::mouseReleaseEvent(QMouseEvent *event)
{
    UCStyledItemBase::mouseReleaseEvent(event);
    Q_D(UCListItem);
    // set released
    if (d->pressed) {
        d->listenToRebind(false);
        if (d->attachedProperties) {
            d->attachedProperties->disableInteractive(this, false);
        }

        d->setContentMoved(true);
        if (!d->suppressClick) {
            Q_EMIT clicked();
            d->_q_rebound();
        } else if (d->contentItem->x() == 0.0) {
            // if no actions list is connected, release flickable blockade
            d->promptRebound();
        } else {
            d->setContentMoved(false);
        }
    }
    d->setPressed(false);
}

void UCListItem::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(UCListItem);
    UCStyledItemBase::mouseMoveEvent(event);

    // accept the tugging only if the move is within the threshold
    bool leadingAttached = UCListItemActionsPrivate::isConnectedTo(d->leadingActions, this);
    bool trailingAttached = UCListItemActionsPrivate::isConnectedTo(d->trailingActions, this);
    if (d->pressed && !(leadingAttached || trailingAttached)) {
        // check if we can initiate the drag at all
        // only X direction matters, if Y-direction leaves the threshold, but X not, the tug is not valid
        qreal threshold = UCUnits::instance().gu(d->xAxisMoveThresholdGU);
        qreal mouseX = event->localPos().x();
        qreal pressedX = d->pressedPos.x();

        if ((mouseX < (pressedX - threshold)) || (mouseX > (pressedX + threshold))) {
            // the press went out of the threshold area, enable move, if the direction allows it
            d->lastPos = event->localPos();
            // tries to connect both panels so we do no longer need to take care which
            // got connected ad which not; this may fail in case of shared ListItemActions,
            // as then the panel is shared between the list items, and the panel might be
            // still in use in other panels. See UCListItemActionsPrivate::connectToListItem
            leadingAttached = d->grabPanel(d->leadingActions, true);
            trailingAttached = d->grabPanel(d->trailingActions, true);
            // create animator if not created yet
            if (!d->animator) {
                d->animator = new UCListItemSnapAnimator(this);
            }
        }
    }

    if (leadingAttached || trailingAttached) {
        qreal x = d->contentItem->x();
        qreal dx = event->localPos().x() - d->lastPos.x();
        d->lastPos = event->localPos();

        if (dx) {
            d->setContentMoved(true);
            // clamp X into allowed dragging area
            d->clampX(x, dx);
            // block flickable
            d->setTugged(true);
            d->contentItem->setX(x);
            // decide which panel is visible by checking the contentItem's X coordinates
            if (d->contentItem->x() > 0) {
                if (leadingAttached) {
                    UCListItemActionsPrivate::get(d->leadingActions)->panelItem->setVisible(true);
                }
                if (trailingAttached) {
                    UCListItemActionsPrivate::get(d->trailingActions)->panelItem->setVisible(false);
                }
            } else if (d->contentItem->x() < 0) {
                // trailing revealed
                if (leadingAttached) {
                    UCListItemActionsPrivate::get(d->leadingActions)->panelItem->setVisible(false);
                }
                if (trailingAttached) {
                    UCListItemActionsPrivate::get(d->trailingActions)->panelItem->setVisible(true);
                }
            }
        }
    }
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
        d->_q_rebound();
        // only accept event, but let it be handled by the underlying or surrounding Flickables
        event->accept();
    }
    return UCStyledItemBase::eventFilter(target, event);
}

/*!
 * \qmlproperty ListItemActions ListItem::leadingActions
 *
 * The property holds the actions and its configuration to be revealed when swiped
 * from left to right.
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
    d->leadingActions = actions;
    Q_EMIT leadingActionsChanged();
}

/*!
 * \qmlproperty ListItemActions ListItem::trailingActions
 *
 * The property holds the actions and its configuration to be revealed when swiped
 * from right to left.
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
    d->trailingActions = actions;
    Q_EMIT trailingActionsChanged();
}

/*!
 * \qmlproperty Item ListItem::contentItem
 *
 * contentItem holds the components placed on a ListItem.
 */
QQuickItem* UCListItem::contentItem() const
{
    Q_D(const UCListItem);
    return d->contentItem;
}

/*!
 * \qmlpropertygroup ::ListItem::divider
 * \qmlproperty bool ListItem::divider.visible
 * \qmlproperty real ListItem::divider.leftMargin
 * \qmlproperty real ListItem::divider.rightMargin
 * \qmlproperty real ListItem::divider.colorFrom
 * \qmlproperty real ListItem::divider.colorTo
 *
 * This grouped property configures the thin divider shown in the bottom of the
 * component. Configures the visibility and the margins from the left and right
 * of the ListItem. When tugged (swiped left or right to reveal the actions),
 * it is not moved together with the content. \c colorFrom and \c colorTo configure
 * the starting and ending colors of the divider.
 *
 * When \c visible is true, the ListItem's content size gets thinner with the
 * divider's \c thickness.
 *
 * The default values for the properties are:
 * \list
 * \li \c visible: true
 * \li \c leftMargin: 2 GU
 * \li \c rightMargin: 2 GU
 * \endlist
 */
UCListItemDivider* UCListItem::divider() const
{
    Q_D(const UCListItem);
    return d->divider;
}

/*!
 * \qmlproperty bool ListItem::pressed
 * True when the item is pressed. The items stays pressed when the mouse or touch
 * is moved horizontally. When in Flickable (or ListView), the item gets un-pressed
 * (false) when the mouse or touch is moved towards the vertical direction causing
 * the flickable to move.
 */
bool UCListItem::pressed() const
{
    Q_D(const UCListItem);
    return d->pressed;
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
 * Configures the color when pressed. Defaults to the theme palette's background color.
 */
QColor UCListItem::highlightColor() const
{
    Q_D(const UCListItem);
    return d->highlightColor;
}
void UCListItem::setHighlightColor(const QColor &color)
{
    Q_D(UCListItem);
    if (d->highlightColor == color) {
        return;
    }
    d->highlightColor = color;
    // no more theme change watch
    d->customColor = true;
    update();
    Q_EMIT highlightColorChanged();
}

/*!
 * \qmlproperty list<Object> ListItem::data
 * \default
 * Overloaded default property containing all the children and resources.
 */
QQmlListProperty<QObject> UCListItem::data()
{
    Q_D(UCListItem);
    return QQuickItemPrivate::get(d->contentItem)->data();
}

/*!
 * \qmlproperty list<Item> ListItem::children
 * Overloaded default property containing all the visible children of the item.
 */
QQmlListProperty<QQuickItem> UCListItem::children()
{
    Q_D(UCListItem);
    return QQuickItemPrivate::get(d->contentItem)->children();
}

/*!
 * \qmlproperty PropertyAnimation ListItem::snap
 * The property holds the animation to be performed on snapping. If set to null,
 * the default animation will be used. Defaults to null.
 */
QQuickPropertyAnimation *UCListItemPrivate::snapAnimation() const
{
    return snap;
}
void UCListItemPrivate::setSnapAnimation(QQuickPropertyAnimation *animation)
{
    if (snap == animation) {
        return;
    }
    snap = animation;
    if (animator) {
        animator->setCustomAnimation(snap);
    }
    Q_Q(UCListItem);
    Q_EMIT q->snapAnimationChanged();
}


#include "moc_uclistitem.cpp"
