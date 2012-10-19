/*
 * Copyright 2012 Canonical Ltd.
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

#include <QtQml/QQmlComponent>
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtCore/QDebug>

#include "itemstyleattached.h"
#include "itemstyleattached_p.h"
#include "rule.h"
#include "themeengine.h"
#include "themeengine_p.h"

const bool traceItemStyleAttached = false;

#ifdef TRACE
#undef TRACE
#endif
#define TRACE \
    if (traceItemStyleAttached) \
        qDebug() << QString("ItemStyleAttached::%1").arg(__FUNCTION__, -15)

#define TRACEP \
    if (traceItemStyleAttached) \
        qDebug() << QString("ItemStyleAttachedPrivate::%1").arg(__FUNCTION__, -15)

const char *itemProperty = "item";
const char *styleProperty = "itemStyle";

/*!
  \qmltype ItemStyle
  \inqmlmodule Ubuntu.Components 0.1
  \ingroup theming
  \brief The ItemStyle attached property contains styling properties for a
  particular UI element.

  The element provides styling support to any element derived from Item
  (QQuickItem).
  The style is selected based on the styleClass and name properties. If neither
  of these is defined, the framework will use the meta class name to identify the
  style rule to be used. However this can happen only if the document defining the
  item refers to the styling attached property.

  The following items will use styling as they declare and reffer to styling
  attached properties, and styling Text can be done by defining the ".Text"
  selector.

  \qml
  Item {
     ItemStyle.styleClass: "button"
  }
  \endqml
  \qml
  Text {
     color: (ItemStyle.style) ? ItemStyle.style.color : "black"
  }
  \endqml

  A item can use private styling by setting the style and/or the deleagte property
  locally. In this case the item won't use the theme defined style/delegate but
  use the styling elements defined locally. Switching back to theme defined styles
  can be achieved by clearing the style/delegate property. It is also possible to
  set only one of the styling elements locally and use a the theme defined one for
  the other.

  \qml
  // Button.qml
  Item {
     id: root
     property bool pressed: false
     property bool hovered: false
     property color color: (ItemStyle.style) ? ItemStyle.style.color : "lightgray"

     signal clicked

     MouseArea {
        anchors.fill: parent
        onClicked: control.clicked()
     }
  }
  \endqml
  In the example above Button document reffers to the style property of the attached
  styling, therefore the element by default will use the style defined using
  ".Button" selector.

  The following example shows a Button item that uses a private delegate but the
  style from the themes.

  \qml
  Button {
     id: control
     ItemStyle.delegate: Rectangle {
        anchors.fill: parent
        color: ItemStyle.style.color
        radius: 15
        border {
            width: 2
            color: Qt.darker(color, 1.4)
        }
     }
  }
  \endqml

  The style is usually applied straight when a styling property is changed. This
  may cause performance problems as there are two properties that can affect the
  style applied. In case the component handles the "Component.onCompleted" signal,
  the styling will be applied only when teh completion occurs. Therefore items
  can handle the completion by simply adding an empty handler to delay styling.
  Modifying teh Button.qml example above, the component that applies styling on
  completion would look as follows:

  \qml
  Item {
     id: root
     property bool pressed: false
     property bool hovered: false
     property color color: (ItemStyle.style) ? ItemStyle.style.color : "lightgray"

     signal clicked

     MouseArea {
        anchors.fill: parent
        onClicked: control.clicked()
     }

     Component.onCompleted:{}
  }
  \endqml

 Attached styling defines two properties in the styling context that can be
 used from delegates to access the item and teh style proeprties. item
 properties can be accessed through "item", and styling properties through
 "itemStyle" proeprty.

*/


ItemStyleAttachedPrivate::ItemStyleAttachedPrivate(ItemStyleAttached *qq, QObject *attached):
    q_ptr(qq),
    attachee(qobject_cast<QQuickItem*>(attached)),
    style(0),
    delegate(0),
    componentContext(0),
    themeRule(0),
    delayApplyingStyle(false),
    customStyle(false),
    customDelegate(false),
    connectedToEngine(false)
{
    // search for a child in the attachee who is derived from QQmlComponentAttached
    // so we can catch the completion
    QObjectList children = attachee->children();
    Q_FOREACH(QObject *child, children) {
        if (child->inherits("QQmlComponentAttached")) {
            TRACEP << attachee->metaObject()->className() << "style update delayed till completion!";
            QObject::connect(child, SIGNAL(completed()), q_ptr, SLOT(_q_refteshStyle()));
            delayApplyingStyle = true;
        }
    }
    listenThemeEngine();

    if (!componentContext) {
        componentContext = new QQmlContext(QQmlEngine::contextForObject(attachee));
        componentContext->setContextProperty(itemProperty, attachee);
    }
}

bool ItemStyleAttachedPrivate::lookupThemeStyle(bool useMetaClassName)
{
    themeRule = ThemeEngine::instance()->lookupStyleRule(attachee, useMetaClassName);
    TRACEP << "useMetaClassName=" << useMetaClassName << themeRule;
    return (themeRule != 0);
}

bool ItemStyleAttachedPrivate::updateStyle()
{
    TRACEP << "ENTER";
    bool result = false;
    // do not do anything till the component gets complete?
    if (delayApplyingStyle)
       return result;

    TRACEP << QString("class: %1, customStyle: %2").arg(styleClass).arg(customStyle);
    if (!customStyle) {
        // check if we have a forced update
        if (style) {
            style->deleteLater();
            style = 0;
        }
        // make sure we have a theme
        result = lookupThemeStyle(false);
        if (!result || (result && !themeRule->style()))
            result = lookupThemeStyle(true);
        if (result) {
            if (style)
                style->deleteLater();
            style = themeRule->createStyle(componentContext);
        }
    }

    // reparent also custom styles!
    if (result && style) {
        style->setParent(attachee);
        componentContext->setContextProperty(styleProperty, style);
    }
    TRACEP << "LEAVE";
    return result;
}

bool ItemStyleAttachedPrivate::updateDelegate()
{
    TRACEP << "ENTER";
    bool result = false;
    // do not do anything till the component gets complete?
    if (delayApplyingStyle)
       return result;

    TRACEP << QString("class: %1, customDelegate: %2").arg(styleClass).arg(customDelegate);

    if (!customDelegate) {
        if (delegate) {
            delegate->deleteLater();
            delegate = 0;
        }
        // make sure we have a theme
        result = lookupThemeStyle(false);
        if (!result || (result && !themeRule->delegate()))
            result = lookupThemeStyle(true);
        if (result) {
            if (delegate)
                delegate->deleteLater();
            delegate = themeRule->createDelegate(componentContext);
        }
    }

    if (delegate && ((delegate->parent() != attachee) || (delegate->parentItem() != attachee))) {
        delegate->setParent(attachee);
        delegate->setParentItem(attachee);
        // If style item contains a property "contentItem" that points
        // to an item, reparent all children into it:
        QVariant contentVariant = delegate->property("contentItem");
        QQuickItem *contentItem = qvariant_cast<QQuickItem *>(contentVariant);
        if (contentItem) {
            Q_FOREACH (QObject *child, attachee->children()) {
                QQuickItem *childItem = qobject_cast<QQuickItem *>(child);
                if (childItem)
                    childItem->setParentItem(contentItem);
            }
        }
    }
    TRACEP << "LEAVE";
    return result;
}

/*!
  \internal
  Updates the style and delegate variables. The style update is forced
  when the item changes the style lookup from private to theme.
*/
void ItemStyleAttachedPrivate::updateCurrentStyle()
{
    bool styleUpdated = updateStyle();
    bool delegateUpdated = updateDelegate();
    if (styleUpdated || delegateUpdated) {
        Q_Q(ItemStyleAttached);
        TRACEP << "emit styleChanged()";
        Q_EMIT q->styleChanged();
    }
}

/*!
  \internal
  Registers the element with the given instance \a id. Returns true on
  successful registration. On error, the theme engine's error string is set.
  */
bool ItemStyleAttachedPrivate::registerName(const QString &id)
{
    bool result = true;
    Q_Q(ItemStyleAttached);
    if (ThemeEngine::instance()->registerName(attachee, id))
        name = id;
    else {
        QString className = q->metaObject()->className();
        className = className.left(className.indexOf("_QMLTYPE"));
        ThemeEnginePrivate::setError(QString("Instance %1 already registered. Resetting instance for %2.")
                                     .arg(name)
                                     .arg(className));
        name = QString();
        result = false;
    }
    return result;
}

/*!
  \internal
  Checks if the item needs to receive updates from theme engine or not. Connects to
  or disconnects from the engine.
  */
void ItemStyleAttachedPrivate::listenThemeEngine()
{
    Q_Q(ItemStyleAttached);
    if (!customStyle || !customDelegate) {
        if (!connectedToEngine)
            connectedToEngine = (bool)QObject::connect(ThemeEngine::instance(), SIGNAL(themeChanged()), q, SLOT(_q_refteshStyle()));
    } else {
        if (connectedToEngine)
            connectedToEngine = !QObject::disconnect(ThemeEngine::instance(), SIGNAL(themeChanged()), q, SLOT(_q_refteshStyle()));
    }
}


/*!
  \internal
  Internal slot to update the style of an item. The slot is connected to the item's
  connected() signal. This is called only if the item defines "Component.onCompleted()"
  attached signal.
  */
void ItemStyleAttachedPrivate::_q_refteshStyle()
{
    TRACEP << QString("%1#%2").arg(styleClass).arg(name);
    // no need to delay style applying any longer
    delayApplyingStyle = false;

    updateCurrentStyle();
}


/*==============================================================================
  */

ItemStyleAttached::ItemStyleAttached(QObject *parent) :
    QObject(parent),
    d_ptr(new ItemStyleAttachedPrivate(this, parent))
{
}

ItemStyleAttached::~ItemStyleAttached()
{}

ItemStyleAttached *ItemStyleAttached::qmlAttachedProperties(QObject *obj)
{
    ItemStyleAttached *ret = 0;
    if (qobject_cast<QQuickItem*>(obj)) {
        ret = new ItemStyleAttached(obj);
    }
    return ret;
}

/*!
  \qmlproperty string ItemStyle::name
  This property holds the item unique identifier used in styling.
  */
/*!
  name property.
  */
QString ItemStyleAttached::name() const
{
    Q_D(const ItemStyleAttached);
    return d->name;
}
/*!
  Updates the name property.
  */
void ItemStyleAttached::setName(const QString &name)
{
    Q_D(ItemStyleAttached);
    if (name != d->name) {
        if (d->registerName(name)) {
            d->listenThemeEngine();
            d->updateCurrentStyle();
        }
    }
}

/*!
  \qmlproperty string ItemStyle::class
  This property holds the style class identifier used by the item.
  When the engine locates the style rule to be applied on the item, it takes
  the styleClass and name properties. If none is specified, the meta class
  name will be used to search for the style. This must be taken into account both
  when defining themes and designing items and applications.
  */
/*!
  class property.
  */
QString ItemStyleAttached::styleClass() const
{
    Q_D(const ItemStyleAttached);
    return d->styleClass;
}
/*!
  Sets the class property value.
  */
void ItemStyleAttached::setStyleClass(const QString &styleClass)
{
    Q_D(ItemStyleAttached);
    if (d->styleClass != styleClass) {
        d->styleClass = styleClass;
        d->listenThemeEngine();
        d->updateCurrentStyle();
    }
}

/*!
  \qmlproperty QtObject ItemStyle::style
  The property holds the object containing the style configuration properties. This can
  either be defined by a theme style rule or the private style. When set, the item will
  no longer use the theme defined style properties but the ones set. The property must be
  reset (set to null object) in order to use th etheme defines styles.

  Modifying the property alone will only affect the styling. The delegate will be used
  from the theme unless specified explicitly. Therefore items can be used with custom
  styling and theme defined delegate, theme style and custom delegate or both theme defined/
  custom.
  */
/*!
  \internal
  Returns the object created out of the style used.
  */
QObject *ItemStyleAttached::style() const
{
    Q_D(const ItemStyleAttached);
    return d->style;
}


/*!
  \internal
  Sets/resets the style object for the item.
  */
void ItemStyleAttached::setStyle(QObject *style)
{
    Q_D(ItemStyleAttached);
    if (d->style != style) {
        // clear the previous style
        if (!d->customStyle && d->style) {
            d->style->deleteLater();
            d->style = 0;
        }
        d->customStyle = (style != 0);
        d->style = style;
        d->listenThemeEngine();
        if (d->updateStyle())
            Q_EMIT styleChanged();
    }
}

/*!
  \qmlproperty Item ItemStyle::delegate
  The property holds the Item containing the visuals of the item defined by
  one of the styles, theme or private. It can be altered
  */
/*!
  Returns the delegate Item created out of the style.
  */
QQuickItem *ItemStyleAttached::delegate() const
{
    Q_D(const ItemStyleAttached);
    return d->delegate;
}

/*!
  \internal
  Sets/resets the delegate item for the item.
  */
void ItemStyleAttached::setDelegate(QQuickItem *delegate)
{
    Q_D(ItemStyleAttached);
    if (d->delegate != delegate) {
        // clear the previous theme delegate
        if (!d->customDelegate && d->delegate) {
            d->delegate->setVisible(false);
            d->delegate->deleteLater();
        }
        d->customDelegate = (delegate != 0);
        d->delegate = delegate;
        d->listenThemeEngine();
        if (d->updateDelegate())
            Q_EMIT styleChanged();
    }
}

#include "moc_itemstyleattached.cpp"
