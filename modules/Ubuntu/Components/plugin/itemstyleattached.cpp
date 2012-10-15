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
#include "stylerule.h"
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

const char *widgetProperty = "widget";
const char *styleProperty = "itemStyle";

/*!
  \qmltype ItemStyle
  \brief The ItemStyle attached property contains styling properties for a
  particular UI element.

  The element provides styling support to any element derived from Item
  (QQuickItem).
  The style is selected based on the styleClass and instanceId properties. If neither
  of these is defined, the framework will use the meta class name to identify the
  style rule to be used. However this can happen only if the document defining the
  widget refers to the styling attached property.

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

  A widget can use private styling by setting the style and/or the deleagte property
  locally. In this case the widget won't use the theme defined style/delegate but
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

  The following example shows a Button widget that uses a private delegate but the
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
  the styling will be applied only when teh completion occurs. Therefore widgets
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
 used from delegates to access the widget and teh style proeprties. Widget
 properties can be accessed through "widget", and styling properties through
 "itemStyle" proeprty.

*/


ItemStyleAttachedPrivate::ItemStyleAttachedPrivate(ItemStyleAttached *qq, QObject *attached):
    q_ptr(qq),
    attachee(qobject_cast<QQuickItem*>(attached)),
    style(0),
    delegate(0),
    componentContext(0),
    styleComponent(0),
    delegateComponent(0),
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
}

/*!
  \internal
  Updates the style and delegate variables. The style update is forced
  when the widget changes the style lookup from private to theme.
*/
void ItemStyleAttachedPrivate::updateCurrentStyle(bool forceUpdate)
{
    Q_Q(ItemStyleAttached);
    // do not do anything till the component gets complete?
    if (delayApplyingStyle)
       return;

    bool defaultChecked = false;
    bool styleChanged = forceUpdate;
    // if force update, we need to clean the previous stuff
    if (forceUpdate) {
        if (!customStyle) {
            if (style) {
                style->deleteLater();
            }
            style = 0;
            styleComponent = 0;
        }
        if (!customDelegate) {
            if (delegate)
                delegate->deleteLater();
            delegate = 0;
            delegateComponent = 0;
        }
    }

    // check if we have a context created, and set the "widget" property so
    // the widget instance can be used from style/delegate
    if (!componentContext) {
        componentContext = new QQmlContext(QQmlEngine::contextForObject(attachee));
        componentContext->setContextProperty(QLatin1String(widgetProperty), attachee);
    }

    TRACEP << QString("class: %1, customStyle: %2, customDelegate: %3").arg(styleClass).arg(customStyle).arg(customDelegate);
    //check if we need to use the theme at all
    if (!customStyle || !customDelegate) {
        StyleRule *rule = ThemeEngine::instance()->lookupStyleRule(attachee);
        if (!rule) {
            // check with class name of the attached property
            TRACEP << "retrieving style for default";
            rule = ThemeEngine::instance()->lookupStyleRule(attachee, true);
            defaultChecked = true;
        } else
            TRACEP << rule->selector();
        if (rule) {
            // apply rule
            // check style
            if (!customStyle && (!style || (styleComponent != rule->style()))) {
                //delete previous object so it doesn't eat memory
                if (style)
                    style->deleteLater();
                styleComponent = rule->style();
                style = rule->createStyle(componentContext);
                styleChanged = true;
            }

            // create delegate
            if (!customDelegate && (!delegate || (delegateComponent != rule->delegate()))) {
                // clear previous delegate so we free some memory
                if (delegate)
                    delegate->deleteLater();
                if (!rule->delegate() && !defaultChecked) {
                    StyleRule * delegateRule = ThemeEngine::instance()->lookupStyleRule(attachee, true);
                    if (delegateRule)
                        rule = delegateRule;
                }
                delegateComponent = rule->delegate();
                delegate = rule->createDelegate(componentContext);
                styleChanged = styleChanged || (delegate != 0);
            }
        }
    }

    //set the style context property
    if (componentContext) {
        componentContext->setContextProperty(styleProperty, style);
    }
    // do the re-parenting fo rstyle and delegate
    if (style && (style->parent() != attachee)) {
        style->setParent(attachee);
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
    if (styleChanged)
        Q_EMIT q->styleChanged();
}

/*!
  \internal
  Registers the element with the given instance \a id. Returns true on
  successful registration. On error, the theme engine's error string is set.
  */
bool ItemStyleAttachedPrivate::registerInstanceId(const QString &id)
{
    bool result = true;
    Q_Q(ItemStyleAttached);
    if (ThemeEngine::instance()->registerInstanceId(attachee, id))
        instanceId = id;
    else {
        QString className = q->metaObject()->className();
        className = className.left(className.indexOf("_QMLTYPE"));
        ThemeEnginePrivate::setError(QString("Instance %1 already registered. Resetting instance for %2.")
                                     .arg(instanceId)
                                     .arg(className));
        instanceId = QString();
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
    TRACEP << styleClass;
    // no need to delay style applying any longer
    delayApplyingStyle = false;

    updateCurrentStyle(true);
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
  \qmlproperty string ItemStyle::instanceId
  This property holds the widget unique identifier used in styling.
  */
/*!
  instanceId property.
  */
QString ItemStyleAttached::instanceId() const
{
    Q_D(const ItemStyleAttached);
    return d->instanceId;
}
/*!
  Updates the instanceId property.
  */
void ItemStyleAttached::setInstanceId(const QString &instanceId)
{
    Q_D(ItemStyleAttached);
    if (instanceId != d->instanceId) {
        if (d->registerInstanceId(instanceId)) {
            d->listenThemeEngine();
            d->updateCurrentStyle();
        }
    }
}

/*!
  \qmlproperty string ItemStyle::styleClass
  This property holds the style class identifier used by the widget.
  When the engine locates the style rule to be applied on the widget, it takes
  the styleClass and instanceId properties. If none is specified, the meta class
  name will be used to search for the style. This must be taken into account both
  when defining themes and designing widgets and applications.
  */
/*!
  styleClass property.
  */
QString ItemStyleAttached::styleClass() const
{
    Q_D(const ItemStyleAttached);
    return d->styleClass;
}
/*!
  Sets the styleClass property value.
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
  from the theme unless specified explicitly. Therefore widgets can be used with custom
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
  Sets/resets the style object for the widget.
  */
void ItemStyleAttached::setStyle(QObject *style)
{
    Q_D(ItemStyleAttached);
    if (d->style != style) {
        d->customStyle = (style != 0);
        // clear the previous style
        if (d->style)
            d->style->deleteLater();
        d->style = style;
        d->listenThemeEngine();
        d->updateCurrentStyle();
    }
}

/*!
  \qmlproperty Item ItemStyle::delegate
  The property holds the Item containing the visuals of the widget defined by
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
  Sets/resets the delegate item for the widget.
  */
void ItemStyleAttached::setDelegate(QQuickItem *delegate)
{
    Q_D(ItemStyleAttached);
    if (d->delegate != delegate) {
        d->customDelegate = (delegate != 0);
        // clear the previous delegate
        if (d->delegate) {
            d->delegate->setVisible(false);
            d->delegate->deleteLater();
        }
        d->delegate = delegate;
        d->listenThemeEngine();
        d->updateCurrentStyle();
    }
}

#include "moc_itemstyleattached.cpp"
