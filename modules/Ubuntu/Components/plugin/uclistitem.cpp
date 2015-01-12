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
#include "i18n.h"
#include "quickutils.h"
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>
#include <QtQuick/private/qquickanimation_p.h>
#include <QtQuick/private/qquickmousearea_p.h>
#include "uclistitemstyle.h"

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
 *
 * The class handles the animation executed when the ListItemAction panel is
 * swiped. The animation is executed from the swipe position the mouse/touch is
 * released to the desired position given in snap(). The action panel is assumed
 * to be anchored to the ListItem.contentItem left or right, depending on which
 * action list is swiped in. Therefore the animator only changes the ListItem.contentItem
 * x coordinate.
 * The animation is defined by the style.
 */
UCListItemSnapAnimator::UCListItemSnapAnimator(UCListItem *item)
    : QObject(item)
    , item(item)
{
}
UCListItemSnapAnimator::~UCListItemSnapAnimator()
{
    // make sure we cannot animate anymore, for safety
    item = 0;
}

/*
 * Snap the ListItem.contentItem in or out, depending on the position specified
 * in "to" parameter. If the position is 0, a snap out will be executed - see
 * snapOut(). In any other cases a snap in action will be performed - see snapIn().
 */
bool UCListItemSnapAnimator::snap(qreal to)
{
    if (!item) {
        return false;
    }
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    QQuickPropertyAnimation *snap = getDefaultAnimation();
    if (!snap) {
        // no animation, so we simply position the component
        listItem->contentItem->setX(to);
        // and complete snap logic
        if (to == 0.0) {
            snapOut();
        } else {
            snapIn();
        }
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
    listItem->setContentMoving(true);
    snap->start();
    return true;
}

/*
 * The function completes a running snap animation.
 */
void UCListItemSnapAnimator::complete()
{
    QQuickPropertyAnimation *snap = getDefaultAnimation();
    if (snap && snap->isRunning()) {
        snap->complete();
    }
}

/*
 * Snap out is performed when the ListItem.contentItem returns back to its original
 * X coordinates (0). At this point both leading and trailing action panels will
 * be disconnected, ascending Flickables will get unlocked (interactive value restored
 * to the state before they were locked) and ListItem.contentMoving will be reset.
 */
void UCListItemSnapAnimator::snapOut()
{
    if (senderSignalIndex() >= 0) {
        // disconnect animation, otherwise snapping will disconnect the panel
        QQuickAbstractAnimation *snap = getDefaultAnimation();
        QObject::disconnect(snap, 0, 0, 0);
    }
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    if (listItem->parentAttached) {
        // restore flickable's interactive and cleanup
        listItem->parentAttached->disableInteractive(item, false);
        // no need to listen flickables any longer
        listItem->listenToRebind(false);
    }
    // disconnect actions
    UCActionPanel::ungrabPanel(listItem->leadingPanel);
    UCActionPanel::ungrabPanel(listItem->trailingPanel);
    // set contentMoved to false
    listItem->setContentMoving(false);
    // lock contentItem left/right edges
    listItem->lockContentItem(true);
}

/*
 * Snap in only resets the ListItem.contentMoving property, but will keep leading/trailing
 * actions connected as well as all ascendant Flickables locked (interactive = false).
 */
void UCListItemSnapAnimator::snapIn()
{
    if (senderSignalIndex() >= 0) {
        // disconnect animation
        QQuickAbstractAnimation *snap = getDefaultAnimation();
        QObject::disconnect(snap, 0, 0, 0);
    }
    // turn content moving off
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    listItem->setContentMoving(false);
}

/*
 * Returns the animation specified by the style.
 */
QQuickPropertyAnimation *UCListItemSnapAnimator::getDefaultAnimation()
{
    UCListItemPrivate *listItem = UCListItemPrivate::get(item);
    listItem->initStyleItem();
    return listItem->styleItem ? listItem->styleItem->m_snapAnimation : 0;
}

/******************************************************************************
 * Divider
 */
UCListItemDivider::UCListItemDivider(QObject *parent)
    : QObject(parent)
    , m_visible(true)
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
    m_thickness = UCUnits::instance().dp(DIVIDER_THICKNESS_DP);
    if (m_listItem) {
        m_listItem->adjustContentItemHeight();
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
    bool lastItem = m_listItem->countOwner ? (m_listItem->index() == (m_listItem->countOwner->property("count").toInt() - 1)): false;
    if (m_visible && !lastItem && (m_gradient.size() > 0) && ((m_colorFrom.alphaF() >= (1.0f / 255.0f)) || (m_colorTo.alphaF() >= (1.0f / 255.0f)))) {
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
    Q_EMIT visibleChanged();
    // set/reset contentItem's bottomMargin
    m_listItem->adjustContentItemHeight();
}

void UCListItemDivider::setLeftMargin(qreal leftMargin)
{
    if (m_leftMargin == leftMargin) {
        return;
    }
    m_leftMargin = leftMargin;
    m_listItem->update();
    Q_EMIT leftMarginChanged();
}

void UCListItemDivider::setRightMargin(qreal rightMargin)
{
    if (m_rightMargin == rightMargin) {
        return;
    }
    m_rightMargin = rightMargin;
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
    , highlighted(false)
    , contentMoved(false)
    , swiped(false)
    , suppressClick(false)
    , ready(false)
    , customColor(false)
    , customOvershoot(false)
    , flicked(false)
    , xAxisMoveThresholdGU(DEFAULT_SWIPE_THRESHOLD_GU)
    , overshoot(0)
    , color(Qt::transparent)
    , highlightColor(Qt::transparent)
    , parentAttached(0)
    , contentItem(new QQuickItem)
    , divider(new UCListItemDivider)
    , leadingActions(0)
    , trailingActions(0)
    , leadingPanel(0)
    , trailingPanel(0)
    , animator(0)
    , styleComponent(0)
    , implicitStyleComponent(0)
    , styleItem(0)
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
                     q, &UCListItem::listItemChildrenChanged);
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

// inspired from IS_SIGNAL_CONNECTED(q, UCListItem, pressAndHold, ())
// the macro cannot be used due to Arguments cannot be an empty ()
bool UCListItemPrivate::isClickedConnected()
{
    Q_Q(UCListItem);
    static QMetaMethod method = QMetaMethod::fromSignal(&UCListItem::clicked);
    static int signalIdx = QMetaObjectPrivate::signalIndex(method);
    return QObjectPrivate::get(q)->isSignalConnected(signalIdx);
}

void UCListItemPrivate::_q_updateThemedData()
{
    Q_Q(UCListItem);
    // we reload the implicit style only if the custom style is not set, and
    // the component is ready
    if (!styleComponent && ready) {
        resetStyle();
    }

    // update colors, panels
    if (!customColor) {
        q->resetHighlightColor();
    }
}

void UCListItemPrivate::_q_rebound()
{
    setHighlighted(false);
    // initiate rebinding only if there were actions tugged
    Q_Q(UCListItem);
    if (!UCActionPanel::isConnected(leadingPanel) &&
        !UCActionPanel::isConnected(trailingPanel)) {
        return;
    }
    setSwiped(false);
    // rebound to zero
    animator->snap(0);
}

void UCListItemPrivate::_q_updateIndex()
{
    Q_Q(UCListItem);
    q->update();
}

/*!
 * \qmlproperty Component ListItem::style
 * Holds the style of the component defining the components visualizing the leading/
 * trailing actions, selection and dragging mode handlers as well as different
 * animations. The component does not assume any visuals present in the style,
 * and will load its content only when requested.
 * \sa ListItemStyle
 */
QQmlComponent *UCListItemPrivate::style() const
{
    return styleComponent ? styleComponent : implicitStyleComponent;
}
void UCListItemPrivate::setStyle(QQmlComponent *delegate)
{
    if (styleComponent == delegate) {
        return;
    }
    Q_Q(UCListItem);
    if (!delegate) {
        // undefined or null delegate resets the style to theme
        resetStyle();
        return;
    }
    // make sure we're rebound before we change the panel component
    promptRebound();
    bool reloadStyle = styleItem != 0;
    if (styleItem) {
        styleItem->deleteLater();
        styleItem = 0;
        Q_EMIT q->__styleInstanceChanged();
    }
    styleComponent = delegate;
    // delete theme style for now
    if (implicitStyleComponent) {
        implicitStyleComponent->deleteLater();
        implicitStyleComponent = 0;
    }
    if (reloadStyle) {
        initStyleItem();
    }
    Q_EMIT q->styleChanged();
}
void UCListItemPrivate::resetStyle()
{
    if (styleComponent || !implicitStyleComponent) {
        styleComponent = 0;
        // rebound as the current panels are not gonna be valid anymore
        if (swiped) {
            promptRebound();
        }
        bool reloadStyle = styleItem != 0;
        if (styleItem) {
            styleItem->deleteLater();
            styleItem = 0;
        }
        delete implicitStyleComponent;
        Q_Q(UCListItem);
        implicitStyleComponent = UCTheme::instance().createStyleComponent("ListItemStyle.qml", q);
        // re-create style instance if it was created using the implicit style
        if (reloadStyle) {
            initStyleItem();
        }
        Q_EMIT q->styleChanged();
    }
}

// creates the style item
void UCListItemPrivate::initStyleItem()
{
    if (!ready || styleItem) {
        return;
    }
    // get the component the style instance is created from
    Q_Q(UCListItem);
    QQmlComponent *delegate = style();
    if (!delegate) {
        // the style is not loaded from the theme yet
        _q_updateThemedData();
        delegate = style();
    }
    if (!delegate) {
        return;
    }
    if (delegate->isError()) {
        qmlInfo(q) << delegate->errorString();
        return;
    }
    QQmlContext *context = new QQmlContext(qmlContext(q));
    QObject *object = delegate->beginCreate(context);
    styleItem = qobject_cast<UCListItemStyle*>(object);
    if (!styleItem) {
        delete object;
        delegate->completeCreate();
        delete context;
        return;
    }
    context->setParent(styleItem);
    QQml_setParent_noEvent(styleItem, q);
    delegate->completeCreate();
    Q_EMIT q->__styleInstanceChanged();

    // get the overshoot value from the style!
    if (!customOvershoot) {
        overshoot = styleItem->m_swipeOvershoot;
        Q_EMIT q->swipeOvershootChanged();
    }
}

/*!
 * \qmlproperty Item ListItem::__styleInstance
 * \internal
 */
QQuickItem *UCListItemPrivate::styleInstance() const
{
    return styleItem;
}

// rebound without animation
void UCListItemPrivate::promptRebound()
{
    setHighlighted(false);
    setSwiped(false);
    if (animator) {
        animator->snapOut();
    }
}

// called when units size changes
void UCListItemPrivate::_q_updateSize()
{
    Q_Q(UCListItem);
    QQuickItem *owner = flickable ? flickable : parentItem;
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

// returns true if the highlight is possible
bool UCListItemPrivate::canHighlight(QMouseEvent *event)
{
    // localPos is a position relative to ListItem which will give us a child from
    // from the original coordinates; therefore we must map the position to the contentItem
   Q_Q(UCListItem);
   QPointF myPos = q->mapToItem(contentItem, event->localPos());
   QQuickItem *child = contentItem->childAt(myPos.x(), myPos.y());
   bool activeComponent = child && (child->acceptedMouseButtons() & event->button()) && !qobject_cast<QQuickText*>(child);
   // do highlight if not pressed above the active component, and the component has either
   // action, leading or trailing actions list or at least an active child component declared
   QQuickMouseArea *ma = q->findChild<QQuickMouseArea*>();
   bool activeMouseArea = ma && ma->isEnabled();
   return !activeComponent && (isClickedConnected() || leadingActions || trailingActions || activeMouseArea);
}

// set highlighted flag and update contentItem
void UCListItemPrivate::setHighlighted(bool highlighted)
{
    if (this->highlighted != highlighted) {
        this->highlighted = highlighted;
        suppressClick = false;
        Q_Q(UCListItem);
        q->update();
        Q_EMIT q->highlightedChanged();
    }
}
// toggles the tugged flag and installs/removes event filter
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
    } else {
        contentAnchors->resetLeft();
        contentAnchors->resetRight();
    }
}

// adjust contentItem height depending on teh divider's visibility
void UCListItemPrivate::adjustContentItemHeight()
{
    QQuickAnchors *contentAnchors = QQuickItemPrivate::get(contentItem)->anchors();
    if (divider->m_visible) {
        contentAnchors->setBottomMargin(divider->m_thickness);
    } else {
        contentAnchors->resetBottomMargin();
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

// clamps the X value and moves the contentItem to the new X value
void UCListItemPrivate::clampAndMoveX(qreal &x, qreal dx)
{
    x += dx;
    // min cannot be less than the trailing's panel width
    QQuickItem *leadingPanelItem = leadingPanel ? leadingPanel->panel() : 0;
    QQuickItem *trailingPanelItem = trailingPanel ? trailingPanel->panel() : 0;
    qreal min = (trailingPanelItem) ? -trailingPanelItem->width() - overshoot: 0;
    // max cannot be bigger than 0 or the leading's width in case we have leading panel
    qreal max = (leadingPanelItem) ? leadingPanelItem->width() + overshoot: 0;
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
 * \l clicked signal. In any other case the component will not be highlighted, and
 * \l highlighted property will not be toggled either. Also, there will be no highlight
 * happening if the click happens on the active component.
 * \qml
 * import QtQuick 2.3
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
 *                text: "No highlight"
 *            }
 *        }
 *    }
 * }
 * \endqml
 *
 * \c contentItem holds all components and resources declared as child to ListItem.
 * Being an Item, all properties can be accessed or altered. However, make sure you
 * never change \c x, \c y, \c width, \c height or \c anchors properties as those are
 * controlled by the ListItem itself when leading or trailing actions are revealed
 * and thus might cause the component to misbehave.
 *
 * Each ListItem has a thin divider shown on the bottom of the component. This
 * divider can be configured through the \c divider grouped property, which can
 * configure its margins from the edges of the ListItem as well as its visibility.
 * When used in \c ListView or \l UbuntuListView, the last list item will not
 * show the divider no matter of the visible property value set.
 *
 * ListItem can handle actions that can get swiped from front to back of the item.
 * These actions are Action elements visualized in panels attached to the front
 * or to the back of the item, and are revealed by swiping the item horizontally.
 * The swipe is started only after the mouse/touch move had passed a given threshold.
 * The actions are visualized by a panel, which is configurable through the \l
 * ListItemStyle::actionsDelegate style property.
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
 */

/*!
 * \qmlsignal ListItem::clicked()
 *
 * The signal is emitted when the component gets released while the \l highlighted property
 * is set. The signal is not emitted if the ListItem content is swiped or when used in
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
    /*
     * Detect the attachee, whether is it a child item of the panelItem. The panelItem
     * itself cannot be detected, as the object can be attached during the call of
     * component.beginCreate().
     */
    UCListItemAttached *attached = new UCListItemAttached(owner);
    QQuickItem *item = qobject_cast<QQuickItem*>(owner);
    while (item) {
        // has item our attached property?
        UCListItemAttached *itemAttached = static_cast<UCListItemAttached*>(
                    qmlAttachedPropertiesObject<UCListItem>(item, false));
        if (itemAttached) {
            attached->connectToAttached(itemAttached);
            break;
        }
        item = item->parentItem();
    }
    return attached;
}

void UCListItem::componentComplete()
{
    UCStyledItemBase::componentComplete();
    Q_D(UCListItem);
    // anchor contentItem prior doing anything else
    QQuickAnchors *contentAnchors = QQuickItemPrivate::get(d->contentItem)->anchors();
    contentAnchors->setTop(d->top());
    contentAnchors->setBottom(d->bottom());
    d->adjustContentItemHeight();
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

        // attach ViewItems to parent item or to ListView
        QQuickItem *parentAttachee = data.item;
        if (d->flickable && d->flickable->inherits("QQuickListView")) {
            // attach to ListView
            d->parentAttached = static_cast<UCViewItemsAttached*>(qmlAttachedPropertiesObject<UCViewItemsAttached>(d->flickable));
            parentAttachee = d->flickable;
        } else if (data.item) {
            d->parentAttached = static_cast<UCViewItemsAttached*>(qmlAttachedPropertiesObject<UCViewItemsAttached>(data.item));
        } else {
            // mark as not ready, so no action should be performed which depends on readyness
            d->ready = false;
            // about to be deleted or reparented, disable attached
            d->parentAttached = 0;
        }

        if (parentAttachee) {
            QObject::connect(parentAttachee, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()), Qt::DirectConnection);
        }

        // update size
        d->_q_updateSize();
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
    if (d->parentAttached && d->parentAttached->isMoving()) {
        // while moving, we cannot select any items
        return;
    }
    if (event->button() == Qt::LeftButton && d->canHighlight(event)) {
        d->setHighlighted(true);
        d->lastPos = d->pressedPos = event->localPos();
        // connect the Flickable to know when to rebound
        d->listenToRebind(true);
        // if it was moved, grab the panels
        if (d->swiped) {
            UCActionPanel::grabPanel(&d->leadingPanel, this, true);
            UCActionPanel::grabPanel(&d->trailingPanel, this, false);
            if (d->parentAttached) {
                d->parentAttached->disableInteractive(this, true);
            }
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
    if (d->highlighted) {
        d->listenToRebind(false);
        if (d->parentAttached) {
            d->parentAttached->disableInteractive(this, false);
        }

        if (!d->suppressClick) {
            Q_EMIT clicked();
            d->_q_rebound();
        }
    }
    d->setHighlighted(false);
}

void UCListItem::mouseMoveEvent(QMouseEvent *event)
{
    Q_D(UCListItem);
    UCStyledItemBase::mouseMoveEvent(event);

    // accept the tugging only if the move is within the threshold
    bool leadingAttached = UCActionPanel::isConnected(d->leadingPanel);
    bool trailingAttached = UCActionPanel::isConnected(d->trailingPanel);
    if (d->highlighted && !(leadingAttached || trailingAttached)) {
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
            leadingAttached = UCActionPanel::grabPanel(&d->leadingPanel, this, true);
            trailingAttached = UCActionPanel::grabPanel(&d->trailingPanel, this, false);
            // unlock contentItem's left/right edges
            d->lockContentItem(false);
            if (d->parentAttached) {
                d->parentAttached->disableInteractive(this, true);
            }
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
            d->setContentMoving(true);
            // clamp X into allowed dragging area
            d->clampAndMoveX(x, dx);
            // block flickable
            d->setSwiped(true);
            d->contentItem->setX(x);
            // decide which panel is visible by checking the contentItem's X coordinates
            if (d->contentItem->x() > 0) {
                if (d->leadingPanel) {
                    d->leadingPanel->panel()->setVisible(true);
                }
                if (d->trailingPanel) {
                    d->trailingPanel->panel()->setVisible(false);
                }
            } else if (d->contentItem->x() < 0) {
                // trailing revealed
                if (d->leadingPanel) {
                    d->leadingPanel->panel()->setVisible(false);
                }
                if (d->trailingPanel) {
                    d->trailingPanel->panel()->setVisible(true);
                }
            }
        }
    }
}

bool UCListItem::childMouseEventFilter(QQuickItem *child, QEvent *event)
{
    QEvent::Type type = event->type();
    if (type == QEvent::MouseButtonPress) {
        // suppress click event if pressed over an active area, except Text, which can also handle
        // mouse clicks when content is an URL
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        if (child->isEnabled() && (child->acceptedMouseButtons() & mouse->button()) && !qobject_cast<QQuickText*>(child)) {
            Q_D(UCListItem);
            d->suppressClick = true;
            // listen for flickable to be able to rebind if movement started there!
            d->listenToRebind(true);
        }
    } else if (type == QEvent::MouseButtonRelease) {
        Q_D(UCListItem);
        d->suppressClick = false;
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
    d->promptRebound();
    // then delete panel
    delete d->leadingPanel;
    d->leadingPanel = 0;
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
    d->promptRebound();
    // then delete panel
    delete d->trailingPanel;
    d->trailingPanel = 0;
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
 * of the ListItem. When swiped left or right to reveal the actions, it is not
 * moved together with the content. \c colorFrom and \c colorTo configure
 * the starting and ending colors of the divider.
 *
 * When \c visible is true, the ListItem's content size gets thinner with the
 * divider's \c thickness.
 *
 * The default values for the properties are:
 * \list
 * \li \c visible: true
 * \li \c leftMargin: 0
 * \li \c rightMargin: 0
 * \endlist
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
    if (d->highlightColor == color) {
        return;
    }
    d->highlightColor = color;
    // no more theme change watch
    d->customColor = true;
    update();
    Q_EMIT highlightColorChanged();
}
void UCListItem::resetHighlightColor()
{
    Q_D(UCListItem);
    d->customColor = false;
    d->highlightColor = getPaletteColor("selected", "background");
    update();
    Q_EMIT highlightColorChanged();
}

/*!
 * \qmlproperty real ListItem::swipeOvershoot
 * The property configures the overshoot value on swiping. Its default value is
 * configured by the \l {ListItemStyle}{style}. Any positive value overrides the
 * default value, and any negative value resets it back to the default.
 */
qreal UCListItemPrivate::swipeOvershoot() const
{
    return overshoot;
}
void UCListItemPrivate::setSwipeOvershoot(qreal overshoot)
{
    // same value should be guarded only if the style hasn't been loaded yet
    // swipeOvershoot can be set to 0 prior the style is loaded.
    if (this->overshoot == overshoot && styleItem) {
        return;
    }
    customOvershoot = (overshoot >= 0.0);
    this->overshoot = (overshoot < 0.0) ?
                // reset, use style to get the overshoot value
                (styleItem ? styleItem->m_swipeOvershoot : 0.0) :
                overshoot;
    update();
    Q_Q(UCListItem);
    Q_EMIT q->swipeOvershootChanged();
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

#include "moc_uclistitem.cpp"
