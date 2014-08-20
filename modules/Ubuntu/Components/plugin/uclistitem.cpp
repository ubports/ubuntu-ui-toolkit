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
#include <QtQml/QQmlInfo>
#include <QtQuick/private/qquickitem_p.h>
#include <QtQuick/private/qquickflickable_p.h>
#include <QtQuick/private/qquickpositioners_p.h>

typedef QList<QQuickGradientStop*> StopList;
/******************************************************************************
 * Divider
 */
UCListItemDivider::UCListItemDivider(QObject *parent)
    : QObject(parent)
    , m_visible(true)
    , m_thickness(UCUnits::instance().dp(1))
    , m_leftMargin(UCUnits::instance().gu(2))
    , m_rightMargin(UCUnits::instance().gu(2))
    , m_color(QColor("#26000000"))
    , m_gradient(0)
    , m_listItem(0)
{
}
UCListItemDivider::~UCListItemDivider()
{
}

void UCListItemDivider::init(UCListItemBase *listItem)
{
    QQml_setParent_noEvent(this, listItem);
    m_listItem = listItem;
}


QSGNode *UCListItemDivider::paint(QSGNode *paintNode, const QRectF &rect)
{
    if (m_visible && (m_color.alpha() != 0 || m_gradient)) {
        QSGRectangleNode *rectNode = static_cast<QSGRectangleNode *>(paintNode);
        if (!rectNode) {
            rectNode = QQuickItemPrivate::get(m_listItem)->sceneGraphContext()->createRectangleNode();
        }
        rectNode->setRect(QRectF(m_leftMargin, rect.height() - m_thickness,
                                 rect.width() - m_leftMargin - m_rightMargin, m_thickness));
        rectNode->setColor(m_color);
        if (m_gradient) {
            rectNode->setGradientStops(m_gradient->gradientStops());
        }
        rectNode->update();
        return rectNode;
    } else {
        delete paintNode;
        return 0;
    }
}

void UCListItemDivider::setVisible(bool visible)
{
    if (m_visible == visible) {
        return;
    }
    m_visible = visible;
    resizeAndUpdate();
    Q_EMIT visibleChanged();
}

void UCListItemDivider::setThickness(qreal thickness)
{
    if (m_thickness == thickness) {
        return;
    }
    m_thickness = thickness;
    resizeAndUpdate();
    Q_EMIT thicknessChanged();
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

void UCListItemDivider::setGradient(QQuickGradient *gradient)
{
    if (m_gradient == gradient) {
        return;
    }
    if (m_gradient) {
        QObject::disconnect(m_gradient, SIGNAL(updated()), m_listItem, SLOT(update()));
    }
    m_gradient = gradient;
    if (m_gradient) {
        QObject::connect(m_gradient, SIGNAL(updated()), m_listItem, SLOT(update()));
        m_listItem->update();
    }
    Q_EMIT gradientChanged();
}

void UCListItemDivider::setColor(const QColor &color)
{
    if (m_color == color) {
        return;
    }
    m_color = color;
    m_listItem->update();
    Q_EMIT colorChanged();
}

/******************************************************************************
 * ListItemBackground
 */
UCListItemBackground::UCListItemBackground(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color(Qt::transparent)
    , m_pressedColor(Qt::yellow)
    , m_item(0)
{
    setFlag(QQuickItem::ItemHasContents);
    // set the z-order to be above the main item
//    setZ(1);
}

UCListItemBackground::~UCListItemBackground()
{
}

void UCListItemBackground::setColor(const QColor &color)
{
    if (m_color == color) {
        return;
    }
    m_color = color;
    update();
    Q_EMIT colorChanged();
}

void UCListItemBackground::setPressedColor(const QColor &color)
{
    if (m_pressedColor == color) {
        return;
    }
    m_pressedColor = color;
    update();
    Q_EMIT pressedColorChanged();
}

void UCListItemBackground::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemParentHasChanged) {
        m_item = qobject_cast<UCListItemBase*>(data.item);
    }
    QQuickItem::itemChange(change, data);
}

QSGNode *UCListItemBackground::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    UCListItemBasePrivate *dd = UCListItemBasePrivate::get(m_item);
    bool pressed = (dd && dd->pressed);
    QColor color = pressed ? m_pressedColor : m_color;

    if (width() <= 0 || height() <= 0 || (color.alpha() == 0)) {
        delete oldNode;
        return 0;
    }
    QSGRectangleNode *rectNode = static_cast<QSGRectangleNode *>(oldNode);
    if (!rectNode) {
        rectNode = QQuickItemPrivate::get(this)->sceneGraphContext()->createRectangleNode();
    }
    rectNode->setColor(color);
    rectNode->setRect(boundingRect());
    rectNode->update();
    return rectNode;
}


/******************************************************************************
 * ListItemBasePrivate
 */
UCListItemBasePrivate::UCListItemBasePrivate()
    : UCFocusScopePrivate()
    , background(new UCListItemBackground)
    , divider(new UCListItemDivider)
{
}
UCListItemBasePrivate::~UCListItemBasePrivate()
{
}

void UCListItemBasePrivate::init()
{
    Q_Q(UCListItemBase);
    background->setObjectName("ListItemHolder");
    QQml_setParent_noEvent(background, q);
    background->setParentItem(q);
    divider->init(q);
    // content will be redirected to the background, therefore we must report
    // children changes as it would come from the main component
    QObject::connect(background, &UCListItemBackground::childrenChanged,
                     q, &UCListItemBase::childrenChanged);
    q->setFlag(QQuickItem::ItemHasContents);
    // turn activeFocusOnMousePress on
    activeFocusOnMousePress = true;
    setFocusable();
}

void UCListItemBasePrivate::setFocusable()
{
    // alsways accept mouse events
    Q_Q(UCListItemBase);
    q->setAcceptedMouseButtons(Qt::LeftButton);
    q->setFiltersChildMouseEvents(true);
}

void UCListItemBasePrivate::_q_rebound()
{
    setPressed(false);
    // disconnect the flickable
    listenToRebind(false);
}

// set pressed flag and update background
void UCListItemBasePrivate::setPressed(bool pressed)
{
    if (this->pressed != pressed) {
        this->pressed = pressed;
        background->update();
        Q_Q(UCListItemBase);
        Q_EMIT q->pressedChanged();
    }
}

// connects/disconnects from the Flickable anchestor to get notified when to do rebound
void UCListItemBasePrivate::listenToRebind(bool listen)
{
    if (flickable.isNull()) {
        return;
    }
    Q_Q(UCListItemBase);
    if (listen) {
        QObject::connect(flickable.data(), SIGNAL(movementStarted()), q, SLOT(_q_rebound()));
    } else {
        QObject::disconnect(flickable.data(), SIGNAL(movementStarted()), q, SLOT(_q_rebound()));
    }
}

void UCListItemBasePrivate::resize()
{
    Q_Q(UCListItemBase);
    QRectF rect(q->boundingRect());
    if (divider && divider->m_visible) {
        rect.setHeight(rect.height() - divider->m_thickness);
    }
    background->setSize(rect.size());
}

/*!
 * \qmltype ListItemBase
 * \instantiates UCListItemBase
 * \inherits Item
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief The ListItem element provides Ubuntu design standards for list or grid
 * views.
 *
 * The component is dedicated to be used in designs with static or dynamic lists
 * (i.e. list views where each item's layout differs or in lists where the content
 * is determined by a given model, thus each element has the same layout). The
 * element does not define any specific layout, components can be placed in any
 * ways on it. However, when used in scrolling lists, the content must be chosen
 * carefully to in order to keep the kinetic behavior and the 60 FPS if possible.
 *
 * The component does not set any size, therefore if used these properties should
 * be set. Colors used are also hardcoded ones. Use \l ListItem instead of this
 * component, which provides bindings to theme palette and aligns to the component
 * is embedded in.
 *
 * \sa ListItem
 */

/*!
 * \qmlsignal ListItemBase::clicked()
 *
 * The signal is emitted when the component gets released while the \l pressed property
 * is set. When used in Flickable, the signal is not emitted if when the Flickable gets
 * moved.
 */
UCListItemBase::UCListItemBase(QQuickItem *parent)
    : UCFocusScope(*(new UCListItemBasePrivate), parent)
{
    Q_D(UCListItemBase);
    d->init();
}

UCListItemBase::~UCListItemBase()
{
}

void UCListItemBase::itemChange(ItemChange change, const ItemChangeData &data)
{
    UCFocusScope::itemChange(change, data);
    if (change == ItemParentHasChanged) {
        Q_D(UCListItemBase);
        // make sure we are not connected to the previous Flickable
        if (!d->flickable.isNull()) {
            QObject::disconnect(d->flickable.data(), SIGNAL(movementStarted()), this, SLOT(_q_rebound()));
        }
        // check if we are in a positioner, and if that positioner is in a Flickable
        QQuickBasePositioner *positioner = qobject_cast<QQuickBasePositioner*>(data.item);
        if (positioner && positioner->parentItem()) {
            d->flickable = qobject_cast<QQuickFlickable*>(positioner->parentItem()->parentItem());
        } else if (data.item && data.item->parentItem()){
            // check if we are in a Flickable then
            d->flickable = qobject_cast<QQuickFlickable*>(data.item->parentItem());
        }

        Q_EMIT owningItemChanged();
    }
}

void UCListItemBase::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    UCFocusScope::geometryChanged(newGeometry, oldGeometry);
    // resize background item
    Q_D(UCListItemBase);
    d->resize();
}

QSGNode *UCListItemBase::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);
    Q_D(UCListItemBase);
    if (width() <= 0 || height() <= 0 || !d->divider) {
        delete oldNode;
        return 0;
    }
    // paint divider
    return d->divider->paint(oldNode, boundingRect());
}

void UCListItemBase::mousePressEvent(QMouseEvent *event)
{
    // skip UCFocusScope mousePressEvent
    QQuickItem::mousePressEvent(event);
    Q_D(UCListItemBase);
    if (!d->flickable.isNull() && d->flickable->isMoving()) {
        // while moving, we cannot select any items
        return;
    }
    d->setPressed(true);
    // connect the Flickable to know when to rebound
    if (!d->flickable.isNull()) {
        QObject::connect(d->flickable.data(), SIGNAL(movementStarted()), this, SLOT(_q_rebound()));
    }
    // accept the event so we get the rest of the events as well
    event->setAccepted(true);
}

void UCListItemBase::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(UCListItemBase);
    // set released
    if (d->pressed) {
        Q_EMIT clicked();
    }
    // save pressed state as UCFocusScope resets it seemlessly
    bool wasPressed = d->pressed;
    UCFocusScope::mouseReleaseEvent(event);
    d->pressed = wasPressed;
    d->setPressed(false);
}

/*!
 * \qmlpropertygroup ::ListItemBase::background
 * \qmlproperty color ListItemBase::background.color
 * \qmlproperty color ListItemBase::background.pressedColor
 *
 * background grouped property is an Item which holds the layout of the item, with
 * abilities to show different colors when in normal state or when pressed. All
 * properties from Item are accessible and can be used to control user defined
 * actions or animations, with the exception of the followings:
 * \list A
 * \li do not alter x, y, width or height properties as those are controlled by the
 *      item itself when leading or trailing options are revealed and will destroy
 *      your logic
 * \li never anchor left or right as it will block revealing the options.
 * \endlist
 */
UCListItemBackground* UCListItemBase::background() const
{
    Q_D(const UCListItemBase);
    return d->background;
}

/*!
 * \qmlpropertygroup ::ListItemBase::divider
 * \qmlproperty bool ListItemBase::divider.visible
 * \qmlproperty real ListItemBase::divider.thickness
 * \qmlproperty real ListItemBase::divider.leftMargin
 * \qmlproperty real ListItemBase::divider.rightMargin
 * \qmlproperty Gradient ListItemBase::divider.gradient
 * \qmlproperty color ListItemBase::divider.color
 *
 * This grouped property configures the thin divider shown in the bottom of the
 * component. Configures the visibility, the thickness, colors and the margins
 * from the left and right of the ListItem. When tugged (swiped left or right to
 * reveal the options), it is not moved together with the content.
 *
 * When \c visible is true, the ListItem's content size gets thinner with the
 * divider's \c thickness.
 *
 * \c color and \c gradient are used to set the color or the gradient the divider
 * should be filled with. The \c gradient has priority over \c color, in the same
 * way as in Rectangle. The following example will draw a gradient between green
 * and yellow colors.
 * \qml
 * Column {
 *     width: units.gu(30)
 *     Repeater {
 *         model: 100
 *         ListItem {
 *             divider {
 *                 color: "blue"
 *                 gradient: Gradient {
 *                     GradientStop {
 *                         color: "green"
 *                         position: 0.0
 *                     }
 *                     GradientStop {
 *                         color: "yellow"
 *                         position: 1.0
 *                     }
 *                 }
 *             }
 *             // ....
 *         }
 *     }
 * }
 * \endqml
 *
 * The default values for the properties are:
 * \list
 * \li \c visible: true
 * \li \c thickness: 1 GU
 * \li \c leftMargin: 2 GU
 * \li \c rightMargin: 2 GU
 * \li \c color: black 15% opacity
 * \li \c gradient: null
 * \endlist
 */
UCListItemDivider* UCListItemBase::divider() const
{
    Q_D(const UCListItemBase);
    return d->divider;
}

/*!
 * \qmlproperty bool ListItemBase::pressed
 * True when the item is pressed. The items stays pressed when the mouse or touch
 * is moved horizontally. When in Flickable (or ListView), the item gets un-pressed
 * (false) when the mouse or touch is moved towards the vertical direction causing
 * the flickable to move.
 */
bool UCListItemBase::pressed() const
{
    Q_D(const UCListItemBase);
    return d->pressed;
}

/*!
 * \qmlproperty Item ListItemBase::owningItem
 * The property contains the Item the component is embedded in. This can be the parent
 * item or the ListView. In the following example the property is the same as parentItem,
 * which is the Column (as Repeater parents all delegates to its parent).
 * \qml
 * Flickable {
 *     id: flickableItem
 *     width: units.gu(30)
 *     height: units.gu(30)
 *     contentHeight: column.childrenRect.height
 *     Column {
 *         id: column
 *         Repeater {
 *             model: 100
 *             ListItem {
 *             }
 *         }
 *     }
 * }
 * \endqml
 * In the following one the owningItem points to the ListView.
 * \qml
 * ListView {
 *     id: listView
 *     width: units.gu(30)
 *     height: units.gu(30)
 *     model: 100
 *     delegate: ListItem {
 *     }
 * }
 * \endqml
 */
QQuickItem *UCListItemBase::owningItem()
{
    // check if we are in a positioner, and if that positioner is in a Flickable
    QQuickBasePositioner *positioner = qobject_cast<QQuickBasePositioner*>(parentItem());
    if (positioner) {
        return positioner;
    }
    Q_D(UCListItemBase);
    return d->flickable;
}

/*!
 * \qmlproperty list<Object> ListItemBase::data
 * \default
 * Overloaded default property containing all the children and resources.
 */
QQmlListProperty<QObject> UCListItemBase::data()
{
    Q_D(UCListItemBase);
    return QQuickItemPrivate::get(d->background)->data();
}

/*!
 * \qmlproperty list<Item> ListItemBase::children
 * Overloaded default property containing all the visible children of the item.
 */
QQmlListProperty<QQuickItem> UCListItemBase::children()
{
    Q_D(UCListItemBase);
    return QQuickItemPrivate::get(d->background)->children();
}

#include "moc_uclistitem.cpp"
