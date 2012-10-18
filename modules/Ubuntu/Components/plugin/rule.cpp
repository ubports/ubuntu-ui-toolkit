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

#include "rule.h"
#include "rule_p.h"
#include "themeengine.h"
#include "themeengine_p.h"
#include <QtQml/QQmlEngine>
#include <QtQml/QQmlContext>
#include <QtCore/QString>

#include <QtCore/QDebug>
const bool traceStyleRule = false;

#ifdef TRACE
#undef TRACE
#endif
#define TRACE \
    if (traceStyleRule) \
        qDebug() << QString("Rule::%1").arg(__FUNCTION__, -15)

#define TRACEP \
    if (traceStyleRule) \
        qDebug() << QString("RulePrivate::%1").arg(__FUNCTION__, -15)

/*!
  \qmltype Rule
  \inqmlmodule Ubuntu.Components 0.1
  \ingroup theming
  \brief The Rule element defines the styling rule to be applied when a item
  satisfies the conditions specified by the rule. The condition is specified by
  the selector property, using the CSS selector syntax.

  The following example defines a style rule that is applied on "frame" style
  class items. items satisfying the condition will use visuals defined in
  the delegate property and configured based on the values set by the style
  component.

  \qml
  // Snapshot of a theme file
  import QtQuick 2.0
  import Ubuntu.Components 0.1

  Item {
      Rule {
        selector: ".frame"
        style: QtObject {
          property color color: "lightgray"
          property color frameColor: "blue"
        }
        delegate: Component {
          Rectangle {
            anchors.fill: item
            color: itemStyle.color
            radius: 5
            border {
              width: 2
              color: itemStyle.frameColor
            }
          }
        }
      }
  }
  \endqml


  Rules can be declared either in a theme or in a ItemStyleAttached, in which case the
  style will be private for the item, and the item won't load any style from
  the themes.

  The theming subsystem declares the context property called "item" through which
  styles can access the targetted item properties, states, signals and functions,
  everything that is declared public in the item.

  There can be cases when the content of the item is anchored to the visuals or
  when the content of a item needs to stand on top of the visuals. In these cases
  delegates can declare an item, which will hold all the item content (item
  children) and name it as "contentItem". The theming engine will search for this
  property and if found, will reparent the item content to it.

  Example:
  \qml

      Rule {
        selector: ".frame"
        style: QtObject {
            property color color: "lightgray"
            property color frameColor: "blue"
        }
        delegate: Component {
            Rectangle {
                property alias contentItem: body
                anchors.fill: item
                color: itemStyle.color
                radius: 5
                border {
                    width: 2
                    color: itemStyle.frameColor
                }
                Item {
                    id: body
                    anchors.fill: parent
                    anchors.margins: 10
                }
            }
        }
      }

  \endqml
  In this example all item content to which the style is applied will get
  reparented to the body item.
*/

/*!
  \preliminary
  \qmlsignal Rule::ruleChanged()
  Emitted when the rule creation gets completed.
*/


RulePrivate::RulePrivate(Rule *qq) :
    q_ptr(qq),
    style(0),
    delegate(0),
    qmlThemeStyle(false)
{
}

/*
  Alternate constructor used in ".qmltheme" styles, when style and delegate component creation may be delayed.
  */
RulePrivate::RulePrivate(Rule *qq, QQmlEngine *engine, const QString &selector, const QString &styleRule, const QString &delegateRule) :
    q_ptr(qq),
    style(0),
    delegate(0),
    selector(selector),
    styleQml(styleRule),
    delegateQml(delegateRule),
    qmlThemeStyle(true)
{
    // create style component
    if (!styleQml.isEmpty())
        createComponent(engine, styleQml, &style);

    // create delegate component
    if (!delegateQml.isEmpty())
        createComponent(engine, delegateQml, &delegate);

    TRACEP << QString("\n=======================================================================\n"
                      "style for [%1]:\n%2\n"
                      "delegate for [%1]:\n%3"
                      "=======================================================================")
              .arg(selector)
              .arg(styleQml)
              .arg(delegateQml);
}

/*
  Destroys style and delegate components in case the rule was created by a ".qmltheme" theme.
  */
RulePrivate::~RulePrivate()
{
    if (qmlThemeStyle) {
        if (!style)
            delete style;
        style = 0;
        if (delegate)
            delete delegate;
        delegate = 0;
    }
}

/*
  Initiates component creation for a given QML code
  */
void RulePrivate::createComponent(QQmlEngine *engine, const QString &qmlCode, QQmlComponent **component)
{
    *component = new QQmlComponent(engine);
    (*component)->setData(qmlCode.toLatin1(), QUrl());
    if ((*component)->isLoading() && !(*component)->isError()) {
        QObject::connect(*component, SIGNAL(statusChanged(QQmlComponent::Status)), q_ptr, SLOT(_q_componentCompleted(QQmlComponent::Status)));
    } else
        completeComponent(*component);
}

/*
  Completes (style or delegate) component creation. If both created style and
  delegate components are ready, emits the ruleChanged signal to update ItemStyleAttacheds.
  */
void RulePrivate::completeComponent(QQmlComponent *sender)
{
    Q_ASSERT_X(sender, Q_FUNC_INFO, "Cannot have NULL component");
    if (sender->isError()) {
        ThemeEnginePrivate::setError(QString("Error on creating rule for selector[%1]: \n%2\n%3")
                                     .arg(selector)
                                     .arg((sender == style) ? styleQml : delegateQml)
                                     .arg(sender->errorString()));
    } else if (!sender->isReady()) {
        ThemeEnginePrivate::setError(QString("Component not ready for selector[%1] \n%2\n%3")
                                     .arg(selector)
                                     .arg((sender == style) ? styleQml : delegateQml)
                                     .arg(sender->errorString()));
    } else {
        bool styleCompleted = false, delegateCompleted = false;
        if (!style || (style && (style->status() == QQmlComponent::Ready)))
            styleCompleted = true;
        if (!delegate || (delegate && (delegate->status() == QQmlComponent::Ready)))
            delegateCompleted = true;
        if (styleCompleted && delegateCompleted) {
            Q_Q(Rule);
            Q_EMIT q->ruleChanged();
        }
    }
}

/*
  Slot completing the component (style or delegate) creation. Calls completeComponent
  with the signal sender.
  */
void RulePrivate::_q_componentCompleted(QQmlComponent::Status)
{
    Q_Q(Rule);
    QQmlComponent *sender = qobject_cast<QQmlComponent*>(q->sender());
    completeComponent(sender);
}

/*=============================================================================
=============================================================================*/

/*!
  \class Rule rule.h

  \internal
*/

/*!
  Creates the rule element. This is used when QML theme file format is loaded.
  */
Rule::Rule(QObject *parent) :
    QObject(parent),
    d_ptr(new RulePrivate(this))
{}

/*!
  Creates rule element used when QTHM format themes are loaded. The style and delegate
  components are created by the rule element as delayed completion can occur.
  */
Rule::Rule(QQmlEngine *engine, const QString &selector, const QString &styleRule, const QString &delegateRule, QObject *parent) :
    QObject(parent),
    d_ptr(new RulePrivate(this, engine, selector, styleRule, delegateRule))
{}

/*!
  */
Rule::~Rule()
{}

/*!
  */
void Rule::classBegin()
{
    // from QQmlParserStatus
}

/*!
  */
void Rule::componentComplete()
{
    // trigger changed signal; this is needed when developer defines a private Rule
    // for a item so property bindings done with the rule are updated
    Q_EMIT ruleChanged();
}

/*!
  Creates a style object in the \a context. Use this method instead of creating
  style objects straight from the style component, as this one checks the readyness
  of the style component of the rule.
  */
QObject *Rule::createStyle(QQmlContext *context)
{
    Q_D(Rule);
    if (d->qmlThemeStyle && d->style && (d->style->status() != QQmlComponent::Ready)) {
        ThemeEnginePrivate::setError(QString("Style not ready for [%1]\n%2")
                                     .arg(d->selector)
                                     .arg(d->styleQml));
        return 0;
    }
    TRACE << QString("create style for %1").arg(d->selector);
    return d->style ? d->style->create(context) : 0;
}

/*!
  Creates the visuals item in the \a context. Use this method instead of creating
  visuals straight from the delegate component, as this one checks the readyness
  of the delegate component of the rule.
  */
QQuickItem *Rule::createDelegate(QQmlContext *context)
{
    Q_D(Rule);
    if (d->qmlThemeStyle && d->delegate && (d->delegate->status() != QQmlComponent::Ready)) {
        ThemeEnginePrivate::setError(QString("Delegate not ready for [%1]\n%2")
                                     .arg(d->selector)
                                     .arg(d->delegateQml));
        return 0;
    }
    TRACE << QString("create delegate for %1").arg(d->selector);
    return d->delegate ? qobject_cast<QQuickItem*>(d->delegate->create(context)) : 0;
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
  Rule selector value
  */
QString Rule::selector() const
{
    Q_D(const Rule);
    return d->selector;
}

/*!
  Sets the rule selector value
  */
void Rule::setSelector(const QString &selector)
{
    Q_D(Rule);
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
  Rule style.
  */
QQmlComponent *Rule::style()
{
    Q_D(const Rule);
    return d->style;
}

/*!
  Sets the style property
  */
void Rule::setStyle(QQmlComponent *style)
{
    Q_D(Rule);
    d->style = style;
}

/*!
  \qmlproperty Component Rule::delegate
  This property holds the visuals component of the style. items implementations
  can decide on the separation of the visuals, either partially or completely.
  It is not mandatory for a item to rely fully on the visuals defined in themes.
  */

/*!
  Rule delegate.
  */
QQmlComponent *Rule::delegate()
{
    Q_D(const Rule);
    return d->delegate;
}

/*!
  Sets the delegate property value
  */
void Rule::setDelegate(QQmlComponent *delegate)
{
    Q_D(Rule);
    d->delegate = delegate;
}

#include "moc_rule.cpp"
