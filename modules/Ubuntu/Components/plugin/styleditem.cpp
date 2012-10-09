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
#include "style.h"
#include "style_p.h"
#include "themeengine.h"
#include "themeengine_p.h"
#include <QtQml/QQmlContext>
#include <QtQml/QQmlEngine>

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
    privateRule(0),
    themeRule(0),
    styleObject(0),
    componentContext(0),
    delegateItem(0),
    componentCompleted(false)
{
}
StyledItemPrivate::~StyledItemPrivate()
{

}

/*!
  \internal
  Updates the styleObject and delegateItem variables. The style update is forced
  when the widget changes the style lookup from private to theme.
*/
void StyledItemPrivate::updateCurrentStyle(bool forceUpdate)
{
    Q_Q(StyledItem);
    // do not do anything till the component gets complete?
    if (!componentCompleted) return;

    bool styleChanged = forceUpdate;
    StyleRule *currentRule = privateRule;

    // in case of private style is in use, no need to change anything
    if (!privateRule) {
        // check whether we have different style for the state
        currentRule = ThemeEngine::instance()->lookupStyleRule(q);
        if (themeRule != currentRule) {
            delete styleObject;
            styleObject = 0;
            themeRule = currentRule;
            styleChanged = true;
        }
    }

    // reset delegate if the style is updated and the new style has visuals
    if (styleChanged && currentRule && currentRule->delegate()) {
        delete delegateItem;
        delegateItem = 0;
    }

    if (styleChanged && currentRule) {
        // check if we have the context
        if (!componentContext) {
            componentContext = new QQmlContext(QQmlEngine::contextForObject(q));
            componentContext->setContextProperty(QLatin1String(widgetProperty), q);
        }

        if (!styleObject && currentRule->style()) {
            styleObject = currentRule->createStyle(componentContext);
            if (styleObject) {
                styleObject->setParent(q);
            }
        }

        // do not mandate yet the existence of visuals
        if (!delegateItem) {
            if (currentRule->delegate())
                delegateItem = currentRule->createDelegate(componentContext);
            else {
                StyleRule *delegateStyle = ThemeEngine::instance()->lookupStyleRule(q, true);
                if (delegateStyle)
                    delegateItem = delegateStyle->createDelegate(componentContext);
            }
            if (delegateItem) {
                // set the styledItem as parent and parentItem too to avoid leaks
                delegateItem->setParent(q);
                delegateItem->setParentItem(q);

                // If style item contains a property "contentItem" that points
                // to an item, reparent all children into it:
                QVariant contentVariant = delegateItem->property("contentItem");
                QQuickItem *contentItem = qvariant_cast<QQuickItem *>(contentVariant);
                if (contentItem) {
                    Q_FOREACH (QObject *child, q->children()) {
                        QQuickItem *childItem = qobject_cast<QQuickItem *>(child);
                        if (childItem)
                            childItem->setParentItem(contentItem);
                    }
                }
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
  Internal slot to update the style of an item. The slot is connected to the theme
  engine's themeChanged() signal.
  */
void StyledItemPrivate::_q_reloadTheme()
{
    // update style if theme is used
    themeRule = 0;
    delete styleObject;
    delete delegateItem;
    styleObject = 0;
    delegateItem = 0;
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
    QObject::connect(ThemeEngine::instance(), SIGNAL(themeChanged()), this, SLOT(_q_reloadTheme()));

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
  \qmlproperty Rule StyledItem::style
  This property holds the private style. When set, the widget will use the styling
  defined in the rule, and altering styleClass and instanceId properties will not
  have any effect on the widget styling. Widgets can be turned back to use theme
  styling by resetting the property.
  */
/*!
  Returns the private style component, null if the widget uses the theme style.
  */
StyleRule *StyledItem::style() const
{
    Q_D(const StyledItem);
    return d->privateRule;
}

/*!
  Sets the private style for the widget.
  */
void StyledItem::setStyle(StyleRule *style)
{
    Q_D(StyledItem);
    if (d->privateRule != style) {
        d->privateRule = style;
        d->_q_reloadTheme();
    }
}

/*!
  Resets the private style to the default, and the widget will use the theme styles.
  */
void StyledItem::resetStyle()
{
    Q_D(StyledItem);
    d->privateRule = 0;
    d->_q_reloadTheme();
}

/*!
  \qmlproperty QtObject StyledItem::styleObject
  The property holds the object containing the style configuration properties. This can
  either be defined by a theme style rule or the private style.
  */
/*!
  Returns the object created out of the style used.
  */
QObject *StyledItem::styleObject() const
{
    Q_D(const StyledItem);
    return d->styleObject;
}

/*!
  \qmlproperty Item StyledItem::delegateItem
  The property holds the Item containing the visuals of the widget defined by
  one of the styles, theme or private.
  */
/*!
  Returns the delegate Item created out of the style.
  */
QQuickItem *StyledItem::delegateItem() const
{
    Q_D(const StyledItem);
    return d->delegateItem;
}


#include "moc_styleditem.cpp"
