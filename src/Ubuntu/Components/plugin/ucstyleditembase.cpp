/*
 * Copyright 2015 Canonical Ltd.
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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#include "ucstyleditembase.h"
#include "ucstyleditembase_p.h"
#include "uctheme.h"
#include "ucstylehints.h"
#include "ucthemingextension.h"
#include "ucnamespace.h"
#include <QtQml/QQmlEngine>
#include <QtQuick/private/qquickanchors_p.h>

UCStyledItemBasePrivate::UCStyledItemBasePrivate()
    : styleComponent(Q_NULLPTR)
    , styleItem(Q_NULLPTR)
    , styleVersion(LATEST_UITK_VERSION)
    , activeFocusOnPress(false)
    , wasStyleLoaded(false)
{
}

UCStyledItemBasePrivate::~UCStyledItemBasePrivate()
{
}

void UCStyledItemBasePrivate::init()
{
    Q_Q(UCStyledItemBase);
    q->setFlag(QQuickItem::ItemIsFocusScope);
}


void UCStyledItemBasePrivate::setFocusable(bool focus)
{
    Q_Q(UCStyledItemBase);
    q->setAcceptedMouseButtons(focus ? (Qt::LeftButton | Qt::RightButton | Qt::MiddleButton) : Qt::NoButton);
    q->setFiltersChildMouseEvents(focus);
}

bool UCStyledItemBasePrivate::isParentFocusable()
{
    if (!activeFocusOnPress) {
        return false;
    }
    QQuickItem *pl = parentItem;
    while (pl) {
        UCStyledItemBase *scope = qobject_cast<UCStyledItemBase*>(pl);
        if (scope) {
            UCStyledItemBasePrivate *pscope = UCStyledItemBasePrivate::get(scope);
            return pscope->isParentFocusable();
        }
        pl = pl->parentItem();
    }

    return true;
}


/*!
 * \qmltype StyledItem
 * \instantiates UCStyledItemBase
 * \inqmlmodule Ubuntu.Components 1.1
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.1
 * \brief The StyledItem class allows items to be styled by the theme.
 *
 * StyledItem provides facilities for making an Item stylable by the theme.
 *
 * In order to make an Item stylable by the theme, it is enough to make the Item
 * inherit from StyledItem and set its \l style property to be the result of the
 * appropriate call to theme.createStyleComponent().
 *
 * Example definition of a custom Item MyItem.qml:
 * \qml
 * StyledItem {
 *     id: myItem
 *     styleName: "MyItemStyle"
 * }
 * \endqml
 *
 * The style set on \l styleName is instantiated and placed below everything else
 * that the Item contains. When the style is defined in this way, the style is taken
 * from the current theme the StyledItem is themed with. Another way tpo set the
 * style of a component is to set the \l style property. This property expects a
 * component and it has precedence over the \l styleName, meaning that when both
 * \l styleName and \l style are set, the style will be created from the \l style
 * componment.
 *
 * A reference to the Item being styled is accessible from the style and named
 * 'styledItem'.
 *
 * \sa {Theme}
 */
UCStyledItemBase::UCStyledItemBase(QQuickItem *parent)
    : QQuickItem(*(new UCStyledItemBasePrivate), parent)
{
    Q_D(UCStyledItemBase);
    d->init();
}

UCStyledItemBase::UCStyledItemBase(UCStyledItemBasePrivate &dd, QQuickItem *parent)
    : QQuickItem(dd, parent)
{
    Q_D(UCStyledItemBase);
    d->init();
}

/*!
 * \qmlmethod void StyledItemBase::requestFocus(Qt::FocusReason reason)
 * \since Ubuntu.Components 1.1
 * The function is similar to \c forceActiveFocus except that it sets focus only
 * if all the ancestors have activeFocusOnPressed set. Returns true if the request
 * succeeded.
 */
bool UCStyledItemBase::requestFocus(Qt::FocusReason reason)
{
    Q_D(UCStyledItemBase);
    bool focusable = d->isParentFocusable();
    if (focusable && isEnabled()) {
        QQuickItem::forceActiveFocus(reason);
    }
    return focusable;
}

/*!
 * \qmlproperty bool StyledItemBase::activeFocusOnPress
 * \since Ubuntu.Components 1.1
 *
 * The property specifies whether the StyledItem can gain focus on a mouse
 * press/touch or not. When the value is true, the focus will be set on the component
 * when the mouse is pressed over it or touched. However if one of the component's
 * ancestor StyledItem or derived is having the property value false, the focus
 * will not be gained automatically.
 *
 * In the following example the TextField will stay focused when clicked on the
 * red rectangle.
 * \qml
 * import QtQuick 2.4
 * import Ubuntu.Components 1.2
 *
 * Column {
 *     width: units.gu(50)
 *     height: units.gu(100)
 *
 *     StyledItem {
 *         objectName: "passiveScope"
 *         width: parent.width
 *         height: units.gu(30)
 *         Rectangle {
 *             anchors.fill: parent
 *             color: "red"
 *             StyledItem {
 *                 objectName: "activeScope"
 *                 activeFocusOnPress: true
 *                 anchors.fill: parent
 *             }
 *         }
 *     }
 *
 *     TextField {
 *         id: input
 *         text: "The input stays focus even if red box is clicked"
 *     }
 *
 *     Component.onCompleted: input.forceActiveFocus()
 *
 *     Connections {
 *         target: window
 *         onActiveFocusItemChanged: console.debug("focus on", window.activeFocusItem)
 *     }
 * }
 * \endqml
 *
 * The default value is \c false.
 */
bool UCStyledItemBase::activefocusOnPress() const
{
    Q_D(const UCStyledItemBase);
    return d->activeFocusOnPress;
}
void UCStyledItemBase::setActiveFocusOnPress(bool value)
{
    Q_D(UCStyledItemBase);
    if (d->activeFocusOnPress == value)
        return;
    d->activeFocusOnPress = value;
    d->setFocusable(d->activeFocusOnPress);
    setActiveFocusOnTab(value);
    Q_EMIT activeFocusOnPressChanged();
}

/*!
 * \qmlproperty Component StyledItem::style
 * Style component instantiated immediately and placed below everything else.
 * Has precedence over the \l styleName. When both set, the style will be
 * created from the component given as property value, and can be reset to the
 * theme style when set to null or reset (set to undefined).
 * \qml
 * Button {
 *     text: "Press to reset"
 *     style: Rectangle {
 *         color: "tan"
 *         Label {
 *             anchors {
 *                 fill: parent
 *                 margins: units.gu(1)
 *             }
 *             text: styledItem.text
 *         }
 *     }
 *     onClicked: style = undefined
 * }
 * \endqml
 */
QQmlComponent *UCStyledItemBasePrivate::style() const
{
    return styleComponent;
}
void UCStyledItemBasePrivate::setStyle(QQmlComponent *style)
{
    if (styleComponent == style) {
        return;
    }
    preStyleChanged();
    styleComponent = style;
    Q_EMIT q_func()->styleChanged();
    postStyleChanged();
    loadStyleItem();
}
void UCStyledItemBasePrivate::resetStyle()
{
    setStyle(Q_NULLPTR);
}

/*!
 * \qmlproperty string StyledItem::styleName
 * The property specifies the component style name. The style name is a document
 * in the current theme, and should not contain the qml file extension. If specified
 * an error will be shown.
 * \qml
 * StyledItem {
 *     id: myItem
 *     styleName: "MyItemStyle"
 * }
 * \endqml
 * \note \l style property has precedence over styleName.
 */
QString UCStyledItemBasePrivate::styleName() const
{
    return styleDocument;
}
void UCStyledItemBasePrivate::setStyleName(const QString &name)
{
    if (name == styleDocument) {
        return;
    }
    QString prevName = styleDocument;
    styleDocument = name;
    if (prevName != styleDocument && !styleComponent) {
        preStyleChanged();
        postStyleChanged();
    }
    Q_EMIT q_func()->styleNameChanged();
    loadStyleItem();
}

// performs pre-style change actions, removes style item size change
// connections and destroys the style component
void UCStyledItemBasePrivate::preStyleChanged()
{
    if (styleItem) {
        // make sure the context holder is reset too
        styleItemContext.clear();
        // disconnect the size changes if they were still connected
        connectStyleSizeChanges(false);
        // remove parentalship to avoid eventual binding loops
        styleItem->setParentItem(NULL);
        // delay deletion to avoid property cache messing
        styleItem->deleteLater();
        styleItem = 0;
    }
}

// loads the style animated or not, depending on the loading time
// returns true on successful style loading
bool UCStyledItemBasePrivate::loadStyleItem(bool animated)
{
    if (styleItem || (!styleComponent && styleDocument.isEmpty()) || !componentComplete) {
        // the style loading is delayed
        return false;
    }
    Q_Q(UCStyledItemBase);
    // either styleComponent or styleName is valid
    QQmlComponent *component = styleComponent;
    if (!component) {
        component = getTheme()->createStyleComponent(styleDocument + ".qml", q, styleVersion);
    }
    if (!component) {
        return false;
    }
    // create context
    // use creation context as parent to create the context we load the style item with
    QQmlContext *creationContext = component->creationContext();
    if (!creationContext) {
        creationContext = qmlContext(q);
    }
    if (creationContext && !creationContext->isValid()) {
        // we are having the changes in the component being under deletion
        return false;
    }
    styleItemContext = new QQmlContext(creationContext);
    styleItemContext->setContextObject(q);
    styleItemContext->setContextProperty("styledItem", q);
    styleItemContext->setContextProperty("animated", animated);
    QObject *object = component->beginCreate(styleItemContext);
    if (!object) {
        delete styleItemContext;
        return false;
    }
    // link context to the style item to delete them together
    QQml_setParent_noEvent(styleItemContext, object);
    styleItem = qobject_cast<QQuickItem*>(object);
    if (styleItem) {
        QQml_setParent_noEvent(styleItem, q);
        styleItem->setParentItem(q);
        // put the style behind evenrything
        styleItem->setZ(-1);
        // anchor fill to the styled component
        QQuickAnchors *styleAnchors = QQuickItemPrivate::get(styleItem)->anchors();
        styleAnchors->setFill(q);
    } else {
        delete object;
    }
    component->completeCreate();
    // delete temporary component
    if (!styleComponent) {
        delete component;
    }

    // make sure we reset the animated property to true
    if (!animated) {
        styleItemContext->setContextProperty("animated", true);
    }

    // set implicit size
    _q_styleResized();
    connectStyleSizeChanges(true);
    Q_EMIT q->styleInstanceChanged();
    return true;
}

/*!
 * \internal
 * Instance of the \l style.
 */
QQuickItem *UCStyledItemBasePrivate::styleInstance()
{
    return styleItem;
}

// connect style item implicit size changes
void UCStyledItemBasePrivate::connectStyleSizeChanges(bool attach)
{
    if (!styleItem) {
        return;
    }
    Q_Q(UCStyledItemBase);
    if (attach) {
        QQuickImplicitSizeItem *sitem = qobject_cast<QQuickImplicitSizeItem*>(styleItem);
        if (sitem) {
            QObject::connect(styleItem, SIGNAL(implicitWidthChanged2()),
                             q, SLOT(_q_styleResized()), Qt::DirectConnection);
            QObject::connect(styleItem, SIGNAL(implicitHeightChanged2()),
                             q, SLOT(_q_styleResized()), Qt::DirectConnection);
        } else {
            QObject::connect(styleItem, SIGNAL(implicitWidthChanged()),
                             q, SLOT(_q_styleResized()), Qt::DirectConnection);
            QObject::connect(styleItem, SIGNAL(implicitHeightChanged()),
                             q, SLOT(_q_styleResized()), Qt::DirectConnection);
        }
    } else {
        QQuickImplicitSizeItem *sitem = qobject_cast<QQuickImplicitSizeItem*>(styleItem);
        if (sitem) {
            QObject::disconnect(styleItem, SIGNAL(implicitWidthChanged2()),
                             q, SLOT(_q_styleResized()));
            QObject::disconnect(styleItem, SIGNAL(implicitHeightChanged2()),
                             q, SLOT(_q_styleResized()));
        } else {
            QObject::disconnect(styleItem, SIGNAL(implicitWidthChanged()),
                             q, SLOT(_q_styleResized()));
            QObject::disconnect(styleItem, SIGNAL(implicitHeightChanged()),
                             q, SLOT(_q_styleResized()));
        }
    }
}

// handle implicit size changes implied by the style components
void UCStyledItemBasePrivate::_q_styleResized()
{
    Q_Q(UCStyledItemBase);
    QObject *sender = q->sender();
    if (sender && sender != styleItem && styleItem) {
        // the implicitSize has been changed by other than the styleItem, detach
        connectStyleSizeChanges(false);
        return;
    }
    qreal w = styleItem ? styleItem->implicitWidth() : 0;
    qreal h = styleItem ? styleItem->implicitHeight() : 0;
    // leave if the new width/height is 0 and the current width/height is not 0
    // and the call was an initial call (sender is not a valid object)
    if ((!w || !h) && !sender && (q->implicitWidth() || q->implicitHeight())) {
        return;
    }
    if (w != implicitWidth) {
        q->setImplicitWidth(w);
    }
    if (h != implicitHeight) {
        q->setImplicitHeight(h);
    }
}

/*!
 * \qmlproperty ThemeSettings StyledItem::theme
 * \since Ubuntu.Components 1.3
 * The property configures the theme the component and all its sub-components
 * will use. By default it is set to the closest ancestor StyledItem's theme
 * if any, or to the system default theme.
 */

void UCStyledItemBasePrivate::preThemeChanged()
{
    wasStyleLoaded = (styleItem != Q_NULLPTR);
    preStyleChanged();
}
void UCStyledItemBasePrivate::postThemeChanged()
{
    Q_EMIT q_func()->themeChanged();
    if (!wasStyleLoaded) {
        return;
    }
    postStyleChanged();
    loadStyleItem();
}

void UCStyledItemBase::classBegin()
{
    QQuickItem::classBegin();
    d_func()->initTheming(this);
}

void UCStyledItemBase::componentComplete()
{
    QQuickItem::componentComplete();
    Q_D(UCStyledItemBase);

    QQmlData *data = QQmlData::get(this);
    QQmlContextData *cdata = QQmlContextData::get(qmlContext(this));
    QQmlPropertyData l;
    QQmlPropertyData *pdata = QQmlPropertyCache::property(qmlEngine(this), this, QStringLiteral("theme"), cdata, l);
    // FIXME MainView internal styler uses theme property, meaning imports13 will be true,
    // therefore we must check the type of the property as well in case anyone else overrides it
    d->styleVersion = data->propertyCache->isAllowedInRevision(pdata) && (property("theme").type() != QVariant::String)
            ? BUILD_VERSION(1, 3)
            : BUILD_VERSION(1, 2);
    // no animation at this time
    // prepare style context if not been done yet
    d->postStyleChanged();
    d->loadStyleItem(false);
}

// grab pressed state and focus if it can be
void UCStyledItemBase::mousePressEvent(QMouseEvent *event)
{
    QQuickItem::mousePressEvent(event);
    requestFocus(Qt::MouseFocusReason);
}

// filter children events as well, so we can catch mouse presses done over child
// MouseAreas or other mouse grabbers
bool UCStyledItemBase::childMouseEventFilter(QQuickItem *child, QEvent *event)
{
    // only filter pressed events
    if (event->type() == QEvent::MouseButtonPress) {
        // send mouse event
        QMouseEvent *mouse = static_cast<QMouseEvent*>(event);
        // the event may occur outside of the parent's boundaries if not clipped
        // therefore must check containment
        QPointF point = mapFromItem(child, mouse->localPos());
        if (contains(point)) {
            requestFocus(Qt::MouseFocusReason);
        }
    }
    // let the event be passed to children
    return QQuickItem::childMouseEventFilter(child, event);
}

// catch UCThemeEvent
void UCStyledItemBase::customEvent(QEvent *event)
{
    Q_D(UCStyledItemBase);
    if (UCThemeEvent::isThemeEvent(event)) {
        d->handleThemeEvent(static_cast<UCThemeEvent*>(event));
    }
}

#include "moc_ucstyleditembase.cpp"
