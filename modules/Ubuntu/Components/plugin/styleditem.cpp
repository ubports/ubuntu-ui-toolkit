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

#include "styleditem.h"
#include "styleditem_p.h"
#include "stylerule.h"
#include "themeengine.h"
#include "themeengine_p.h"
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>
#include <QtCore/QDebug>

const bool traceStyledItem = false;

#ifdef TRACE
#undef TRACE
#endif
#define TRACE \
    if (traceStyledItem) \
        qDebug() << QString("StyledItem::%1").arg(__FUNCTION__, -15)

#define TRACEP \
    if (traceStyledItem) \
        qDebug() << QString("StyledItemPrivate::%1").arg(__FUNCTION__, -15)

const char *widgetProperty = "widget";
/*!
  \qmltype StyledItem
  \brief StyledItem element provides support for widget styling.

  Widgets with styling support can either use this element as base or can declare
  it as internal element, depending on what is the base class to be extended. For
  instance in case of a frame or button it is worth to use StyledItem as base element
  and build the API above it, but in case of a Label element it is worth to declare
  an internal StyledItem element so the published API will be based on Text element.

  The style is selected based on the styleClass and instanceId properties. If neither
  of these is defined, the framework will use the meta class name to identify the
  style rule to be used.

  A widget can use private styling by setting the style property locally. In this case
  the widget won't react on theme changes and altering styleClass or instanceId properties
  will not cause style changes either. Switch back to theme defined styles can be
  achieved by clearing the style property.

  \qml
  // Button.qml
  StyledItem {
     id: root
     property bool pressed: false
     property bool hovered: false

     signal clicked

     MouseArea {
        anchors.fill: parent
        onClicked: control.clicked()
     }
  }
  \endqml
  In this example Button element is derived from StyledItem, and uses "Button" as
  default style class to be selected.

  \qml
  // Label.qml
  Text {
     id: control
     property alias styleClass: internal.styleClass
     property alias instanceId: internal.instanceId
     property alias style: internal.style
     StyledItem {
        id: internal
        onStyleChanged: {
           // update label's properties using style configuration
        }
     }
  }
  \endqml
  In this example the Label element extends the default Text element with styling
  by declaring the StyledItem element internally and exposing its styleClass and
  instanceId properties.
*/

StyledItemPrivate::StyledItemPrivate(StyledItem *qq):
    q_ptr(qq),
    style(0),
    delegate(0),
    componentContext(0),
    styleComponent(0),
    delegateComponent(0),
    componentCompleted(false),
    customStyle(false),
    customDelegate(false),
    connectedToEngine(false)
{
}
StyledItemPrivate::~StyledItemPrivate()
{

}

/*!
  \internal
  Updates the style and delegate variables. The style update is forced
  when the widget changes the style lookup from private to theme.
*/
void StyledItemPrivate::updateCurrentStyle(bool forceUpdate)
{
    Q_Q(StyledItem);
    // do not do anything till the component gets complete?
    if (!componentCompleted)
       return;

    TRACEP << QString("class: %1, customStyle: %2, customDelegate: %3").arg(styleClass).arg(customStyle).arg(customDelegate);
    bool styleChanged = forceUpdate;
    // if force update, we need to clean the previous stuff
    if (forceUpdate) {
        if (!customStyle) {
            delete style;
            style = 0;
            styleComponent = 0;
        }
        if (!customDelegate) {
            delete delegate;
            delegate = 0;
            delegateComponent = 0;
        }
    }

    //check if we need to use the theme at all
    if (!customStyle || !customDelegate) {
        StyleRule *rule = ThemeEngine::instance()->lookupStyleRule(q);
        if (!rule) {
            // no style found, exit
            return;
        }

        // check if we have a context created, and set the "widget" property so
        // the widget instance can be used from style/delegate
        if (!componentContext) {
            componentContext = new QQmlContext(QQmlEngine::contextForObject(q));
            componentContext->setContextProperty(QLatin1String(widgetProperty), q);
        }

        // check them separately
        if (!customStyle && (!style || (styleComponent != rule->style()))) {
            //delete previous object so it doesn't eat memory
            delete style;
            styleComponent = rule->style();
            style = rule->createStyle(componentContext);
            if (style)
                style->setParent(q);
            styleChanged = true;
        } else if (customStyle)
            style->setParent(q);

        // create delegate
        if (!customDelegate && (!delegate || (delegateComponent != rule->delegate()))) {
            // clear previous delegate so we free some memory
            delete delegate;
            delegateComponent = rule->delegate();
            delegate = rule->createDelegate(componentContext);
            styleChanged = styleChanged || (delegate != 0);
            if (delegate) {
                // set the styledItem as parent and parentItem too to avoid leaks
                delegate->setParent(q);
                delegate->setParentItem(q);

                // If style item contains a property "contentItem" that points
                // to an item, reparent all children into it:
                QVariant contentVariant = delegate->property("contentItem");
                QQuickItem *contentItem = qvariant_cast<QQuickItem *>(contentVariant);
                if (contentItem) {
                    Q_FOREACH (QObject *child, q->children()) {
                        QQuickItem *childItem = qobject_cast<QQuickItem *>(child);
                        if (childItem)
                            childItem->setParentItem(contentItem);
                    }
                }
            }
        } else if (customDelegate) {
            // do the reparenting!
            delegate->setParent(q);
            delegate->setParentItem(q);
        }
    } else {
        //reparent custom style and delegate
        if (customStyle)
            style->setParent(q);
        if (customDelegate) {
            delegate->setParent(q);
            delegate->setParentItem(q);
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
bool StyledItemPrivate::registerInstanceId(const QString &id)
{
    bool result = true;
    Q_Q(StyledItem);
    if (ThemeEngine::instance()->registerInstanceId(q, id))
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
void StyledItemPrivate::listenThemeEngine()
{
    Q_Q(StyledItem);
    if (!customStyle || !customDelegate) {
        if (!connectedToEngine)
            connectedToEngine = (bool)QObject::connect(ThemeEngine::instance(), SIGNAL(themeChanged()), q, SLOT(_q_reloadTheme()));
    } else {
        if (connectedToEngine)
            connectedToEngine = !QObject::disconnect(ThemeEngine::instance(), SIGNAL(themeChanged()), q, SLOT(_q_reloadTheme()));
    }
}


/*!
  \internal
  Internal slot to update the style of an item. The slot is connected to the theme
  engine's themeChanged() signal. This is called only if the item uses style rules
  from the theme.
  */
void StyledItemPrivate::_q_reloadTheme()
{
    // update style if theme is used
    if (!customStyle) {
        styleComponent = 0;
        delete style;
        style = 0;
    }
    if (!customDelegate) {
        delegateComponent = 0;
        delete delegate;
        delegate = 0;
    }
    updateCurrentStyle(true);
}

/*-----------------------------------------------------------------------------
-----------------------------------------------------------------------------*/
/*!
  Creates a styledItem instance
  */
StyledItem::StyledItem(QQuickItem *parent) :
    QQuickItem(parent),
    d_ptr(new StyledItemPrivate(this))
{
}

/*!
  Destroys the StyledItem object
  */
StyledItem::~StyledItem()
{}

/*!
  Updates the widget with the defined style.
  */
void StyledItem::componentComplete()
{
    Q_D(StyledItem);

    QQuickItem::componentComplete();

    // connect to theme engine's themeChanged() signal to refresh style when
    // theme is updated
    d->listenThemeEngine();

    // check whether setting instanceId is possible
    d->registerInstanceId(d->instanceId);

    d->componentCompleted = true;

    // activate style and force update as this is the first time style is
    // taken into account
    d->updateCurrentStyle(true);
}

/*!
  \qmlproperty string StyledItem::instanceId
  This property holds the widget unique identifier used in styling.
  */
/*!
  instanceId property.
  */
QString StyledItem::instanceId() const
{
    Q_D(const StyledItem);
    return d->instanceId;
}
/*!
  Updates the instanceId property.
  */
void StyledItem::setInstanceId(const QString &instanceId)
{
    Q_D(StyledItem);
    if (instanceId != d->instanceId) {
        if (!d->componentCompleted) {
            // do the checking once completed
            d->instanceId = instanceId;
        } else if (d->registerInstanceId(instanceId))
            d->updateCurrentStyle();
    }
}

/*!
  \qmlproperty string StyledItem::styleClass
  This property holds the style class identifier used by the widget.
  When the engine locates the style rule to be applied on the widget, it takes
  the styleClass and instanceId properties. If none is specified, the meta class
  name will be used to search for the style. This must be taken into account both
  when defining themes and designing widgets and applications.
  */
/*!
  styleClass property.
  */
QString StyledItem::styleClass() const
{
    Q_D(const StyledItem);
    return d->styleClass;
}
/*!
  Sets the styleClass property value.
  */
void StyledItem::setStyleClass(const QString &styleClass)
{
    Q_D(StyledItem);
    if (d->styleClass != styleClass) {
        d->styleClass = styleClass;
        d->updateCurrentStyle();
    }
}

/*!
  \qmlproperty QtObject StyledItem::style
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
QObject *StyledItem::style() const
{
    Q_D(const StyledItem);
    return d->style;
}


/*!
  \internal
  Sets/resets the style object for the widget.
  */
void StyledItem::setStyle(QObject *style)
{
    Q_D(StyledItem);
    if (d->style != style) {
        d->customStyle = (style != 0);
        d->style = style;
        d->listenThemeEngine();
        d->updateCurrentStyle();
    }
}

/*!
  \qmlproperty Item StyledItem::delegate
  The property holds the Item containing the visuals of the widget defined by
  one of the styles, theme or private. It can be altered
  */
/*!
  Returns the delegate Item created out of the style.
  */
QQuickItem *StyledItem::delegate() const
{
    Q_D(const StyledItem);
    return d->delegate;
}

/*!
  \internal
  Sets/resets the delegate item for the widget.
  */
void StyledItem::setDelegate(QQuickItem *delegate)
{
    Q_D(StyledItem);
    if (d->delegate != delegate) {
        d->customDelegate = (delegate != 0);
        d->delegate = delegate;
        d->listenThemeEngine();
        d->updateCurrentStyle();
    }
}

#include "moc_styleditem.cpp"
