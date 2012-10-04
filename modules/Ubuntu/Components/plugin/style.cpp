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

#include "style.h"
#include "style_p.h"
#include "themeengine.h"
#include <QQmlEngine>
#include <QQmlContext>

/*!
  \qmlclass Rule StyleRule
  \brief The Rule element defines the styling rule to be applied when a widget
  satisfies the conditions specified by the rule. The condition is specified by
  the selector property, using the CSS selector syntax.

  The following example defines a style rule that is applied on "frame" style
  class widgets. Widgets satisfying the condition will use visuals defined in
  the delegate property and configured based on the values set by the style
  component.

  \qml
  Item {
      Rule {
        selector: ".frame"
        style: QtObject {
          property color color: "lightgray"
          property color frameColor: "blue"
        }
        delegate: Component {
          Rectangle {
            anchors.fill: widget
            color: widget.styleObject.color
            radius: 5
            border {
              width: 2
              color: widget.styleObject.frameColor
            }
          }
        }
      }
  }
  \endqml


  Rules can be declared either in a theme or in a StyledItem, in which case the
  style will be private for the widget, and the widget won't load any style from
  the themes.

  The theming subsystem declares the context property called "widget" through which
  styles can access the targetted widget properties, states, signals and functions,
  everything that is declared public in the widget.

  There can be cases when the content of the widget is anchored to the visuals or
  when the content of a widget needs to stand on top of the visuals. In these cases
  delegates can declare an item, which will hold all the widget content (widget
  children) and name it as "contentItem". The theming engine will search for this
  property and if found, will reparent the widget content to it.

  Example:
  \qml
  Item {
      Rule {
        selector: ".frame"
        style: QtObject {
            property color color: "lightgray"
            property color frameColor: "blue"
        }
        delegate: Component {
            Rectangle {
                property alias contentItem: body
                anchors.fill: widget
                color: widget.styleObject.color
                radius: 5
                border {
                    width: 2
                    color: widget.styleObject.frameColor
                }
                Item {
                    id: body
                    anchors.fill: parent
                    anchors.margins: 10
                }
            }
        }
      }
  }
  \endqml
  In this example all widget content to which the style is applied will get
  reparented to the body item.
*/

StyleRulePrivate::StyleRulePrivate(StyleRule *qq) :
    q_ptr(qq),
    style(0),
    delegate(0)
{
}

/*=============================================================================
=============================================================================*/

/*!
  Creates the rule element
  */
StyleRule::StyleRule(QObject *parent) :
    QObject(parent),
    d_ptr(new StyleRulePrivate(this))
{}

StyleRule::~StyleRule()
{}


void StyleRule::classBegin()
{

}

void StyleRule::componentComplete()
{
    // Q_EMIT changed signal; this is needed when developer defines a private StyleRule
    // for a control
    Q_EMIT ruleChanged();
}

/*!
  \qmlproperty string Rule::selector
  This property holds the rule selector. The selector syntax follows some elements
  of the CSS-selector syntax as specified in W3C specifications as follows:
    - Type selectors, e.g: "Button"
    - Descendant selectors, e.g: "Dialog Button"
    - Child selectors, e.g: "Dialog > Button"
    - ID selectors, e.g: "Button#mySpecialButton"
    - Grouping, e.g: "Button#foo, Checkbox, #bar"
  */

/*!
  \property StyleRule::selector
  Rule selector value
  */
QString StyleRule::selector() const
{
    Q_D(const StyleRule);
    return d->selector;
}

/*!
  Sets the rule selector value
  */
void StyleRule::setSelector(const QString &selector)
{
    Q_D(StyleRule);
    d->selector = selector;
}

/*!
  \qmlproperty Component Rule::style
  This property holds the style configuration component. The style configuration
  component usually is a QtObject or a derivate of a QtObject, which enumerates
  the properties that can be configured on the rule delegate. It is not mandatory
  for a rule to have style or delegate, however one of these properties must be
  configured on the rule.
  */
/*!
  \property StyleRule::style
  */
QQmlComponent *StyleRule::style()
{
    Q_D(const StyleRule);
    return d->style;
}

/*!
  Sets the style property
  */
void StyleRule::setStyle(QQmlComponent *style)
{
    Q_D(StyleRule);
    d->style = style;
}

/*!
  \qmlproperty Component Rule::delegate
  This property holds the visuals component of the style. Widgets implementations
  can decide on the separation of the visuals, either partially or completely.
  It is not mandatory for a widget to rely fully on the visuals defined in themes.
  */

/*!
  \property StyleRule::delegate
  */
QQmlComponent *StyleRule::delegate()
{
    Q_D(const StyleRule);
    return d->delegate;
}

/*!
  Sets the delegate property value
  */
void StyleRule::setDelegate(QQmlComponent *delegate)
{
    Q_D(StyleRule);
    d->delegate = delegate;
}

#include "moc_style.cpp"
