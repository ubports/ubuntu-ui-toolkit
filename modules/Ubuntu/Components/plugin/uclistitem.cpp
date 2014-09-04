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
 * ListItemBackground
 */
UCListItemBackground::UCListItemBackground(QQuickItem *parent)
    : QQuickItem(parent)
    , m_color(Qt::transparent)
    , m_pressedColor(Qt::yellow)
    , m_item(0)
{
    setFlag(QQuickItem::ItemHasContents);
    // catch theme palette changes
    connect(&UCTheme::instance(), &UCTheme::paletteChanged, this, &UCListItemBackground::updateColors);
    updateColors();
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
    // no more theme change watch
    disconnect(&UCTheme::instance(), &UCTheme::paletteChanged, this, &UCListItemBackground::updateColors);
    update();
    Q_EMIT pressedColorChanged();
}

void UCListItemBackground::updateColors()
{
    m_pressedColor = getPaletteColor("selected", "background");
    update();
}


void UCListItemBackground::itemChange(ItemChange change, const ItemChangeData &data)
{
    if (change == ItemParentHasChanged) {
        m_item = qobject_cast<UCListItem*>(data.item);
    }
    QQuickItem::itemChange(change, data);
}

QSGNode *UCListItemBackground::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
    Q_UNUSED(data);

    UCListItemPrivate *dd = UCListItemPrivate::get(m_item);
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


UCListItemPrivate::UCListItemPrivate()
    : UCStyledItemBasePrivate()
    , pressed(false)
    , background(new UCListItemBackground)
{
}
UCListItemPrivate::~UCListItemPrivate()
{
}

void UCListItemPrivate::init()
{
    Q_Q(UCListItem);
    background->setObjectName("ListItemHolder");
    QQml_setParent_noEvent(background, q);
    background->setParentItem(q);
    // content will be redirected to the background, therefore we must report
    // children changes as it would come from the main component
    QObject::connect(background, &UCListItemBackground::childrenChanged,
                     q, &UCListItem::childrenChanged);
    q->setFlag(QQuickItem::ItemHasContents);
    // turn activeFocusOnPress on
    activeFocusOnPress = true;
    setFocusable();

    // watch size change and set implicit size;
    QObject::connect(&UCUnits::instance(), SIGNAL(gridUnitChanged()), q, SLOT(_q_updateSize()));
    _q_updateSize();
}

void UCListItemPrivate::setFocusable()
{
    // alsways accept mouse events
    Q_Q(UCListItem);
    q->setAcceptedMouseButtons(Qt::LeftButton | Qt::MiddleButton | Qt::RightButton);
    q->setFiltersChildMouseEvents(true);
}

void UCListItemPrivate::_q_rebound()
{
    setPressed(false);
    // disconnect the flickable
    listenToRebind(false);
}

// called when units size changes
void UCListItemPrivate::_q_updateSize()
{
    Q_Q(UCListItem);
    QQuickItem *owner = flickable ? flickable : parentItem;
    q->setImplicitWidth(owner ? owner->width() : UCUnits::instance().gu(40));
    q->setImplicitHeight(UCUnits::instance().gu(6));
}

// set pressed flag and update background
void UCListItemPrivate::setPressed(bool pressed)
{
    if (this->pressed != pressed) {
        this->pressed = pressed;
        background->update();
        Q_Q(UCListItem);
        Q_EMIT q->pressedChanged();
    }
}

// connects/disconnects from the Flickable anchestor to get notified when to do rebound
void UCListItemPrivate::listenToRebind(bool listen)
{
    if (flickable.isNull()) {
        return;
    }
    if (listen) {
        QObject::connect(flickable.data(), SIGNAL(movementStarted()), q_ptr, SLOT(_q_rebound()));
    } else {
        QObject::disconnect(flickable.data(), SIGNAL(movementStarted()), q_ptr, SLOT(_q_rebound()));
    }
}

/*!
 * \qmltype ListItem
 * \instantiates UCListItem
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \brief The ListItem element provides Ubuntu design standards for list or grid
 * views.
 *
 * The component is dedicated to be used in designs with static or dynamic lists
 * (i.e. list views where each item's layout differs or in lists where the content
 * is determined by a given model, thus each element has the same layout). The
 * element does not define any specific layout, components can be placed in any
 * ways on it. However, when used in list views, the content must be carefully
 * chosen to in order to keep the kinetic behavior and the highest FPS possible.
 */

/*!
 * \qmlsignal ListItem::clicked()
 *
 * The signal is emitted when the component gets released while the \l pressed property
 * is set. When used in Flickable, the signal is not emitted if when the Flickable gets
 * moved.
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

        if (d->flickable) {
            // connect to flickable to get width changes
            QObject::connect(d->flickable, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
        } else if (data.item) {
            QObject::connect(data.item, SIGNAL(widthChanged()), this, SLOT(_q_updateSize()));
        }

        // update size
        d->_q_updateSize();
    }
}

void UCListItem::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    UCStyledItemBase::geometryChanged(newGeometry, oldGeometry);
    // resize background item
    Q_D(UCListItem);
    QRectF rect(boundingRect());
    d->background->setSize(rect.size());
}
void UCListItem::mousePressEvent(QMouseEvent *event)
{
    UCStyledItemBase::mousePressEvent(event);
    Q_D(UCListItem);
    if (!d->flickable.isNull() && d->flickable->isMoving()) {
        // while moving, we cannot select any items
        return;
    }
    d->setPressed(true);
    // connect the Flickable to know when to rebound
    d->listenToRebind(true);
    // accept the event so we get the rest of the events as well
    event->setAccepted(true);
}

void UCListItem::mouseReleaseEvent(QMouseEvent *event)
{
    Q_D(UCListItem);
    // set released
    if (d->pressed) {
        Q_EMIT clicked();
    }
    // save pressed state as UCFocusScope resets it seemlessly
    bool wasPressed = d->pressed;
    UCStyledItemBase::mouseReleaseEvent(event);
    d->pressed = wasPressed;
    d->setPressed(false);
}

/*!
 * \qmlpropertygroup ::ListItem::background
 * \qmlproperty color ListItem::background.color
 * \qmlproperty color ListItem::background.pressedColor
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
UCListItemBackground* UCListItem::background() const
{
    Q_D(const UCListItem);
    return d->background;
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
 * \qmlproperty list<Object> ListItem::data
 * \default
 * Overloaded default property containing all the children and resources.
 */
QQmlListProperty<QObject> UCListItem::data()
{
    Q_D(UCListItem);
    return QQuickItemPrivate::get(d->background)->data();
}

/*!
 * \qmlproperty list<Item> ListItem::children
 * Overloaded default property containing all the visible children of the item.
 */
QQmlListProperty<QQuickItem> UCListItem::children()
{
    Q_D(UCListItem);
    return QQuickItemPrivate::get(d->background)->children();
}

#include "moc_uclistitem.cpp"
