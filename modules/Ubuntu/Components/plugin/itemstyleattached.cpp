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
#include <QtQml/QQmlInfo>
#include <QtQuick/QQuickItem>

#include "itemstyleattached.h"
#include "itemstyleattached_p.h"
#include "themeengine.h"
#include "themeengine_p.h"
#include "quickutils.h"

#include <private/qqmlproperty_p.h>

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
  The style is selected based on the \a class and \a name properties. If neither
  of these is defined, the framework will use the meta class name to identify the
  style rule to be used. However this can happen only if the document defining the
  item refers to the styling attached property.

  The following items will use styling as they declare and refer to styling
  attached properties, and styling Text can be done by defining the ".Text"
  selector.

  \qml
  Item {
      ItemStyle.class: "button"
  }
  \endqml
  \qml
  Text {
      ItemStyle.class: "text"
  }
  \endqml

  An item can use private styling by setting the style and/or the delegate property
  locally. In this case the item won't use the theme defined style/delegate but will
  use the styling elements defined locally. Switching back to theme defined styles
  can be achieved by clearing the style/delegate property. It is also possible to
  set only one of the styling elements locally and use the theme defined one for
  the other.

  \qml
  // Button.qml
  Item {
     id: root
     property bool pressed: false
     property bool hovered: false
     property color color: "lightgray"

     signal clicked

     MouseArea {
        anchors.fill: parent
        onClicked: control.clicked()
     }
  }
  \endqml
  In the example above the Button document refers to the style property of the attached
  styling, therefore the element by default will use the style defined using
  the ".button" selector.

  The following example shows a Button item that uses a private delegate but the
  styles from the themes.

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

 Attached styling defines a context property which can be used from delegates and
 also in style declaration to access the item properties. This property is called
 \b item.
*/


ItemStyleAttachedPrivate::ItemStyleAttachedPrivate(ItemStyleAttached *qq, QObject *attached):
    q_ptr(qq),
    attachee(qobject_cast<QQuickItem*>(attached)),
    style(0),
    delegate(0),
    componentContext(new QQmlContext(QQmlEngine::contextForObject(attachee))),
    styleRule(0),
    delayApplyingStyle(true),
    customStyle(false),
    customDelegate(false),
    connectedToEngine(false)
{
    styleClass = QuickUtils::instance().className(attachee).toLower();
    // refresh style upon reparenting!
    // there is no reason to do styling till the parent is not set and this applies
    // to the root objects too as even those have an internal parent
    QObject::connect(attachee, SIGNAL(parentChanged(QQuickItem*)), q_ptr, SLOT(_q_reapplyStyling(QQuickItem*)));

    listenThemeEngine();

    componentContext->setContextProperty(itemProperty, attachee);

    //enum attachee properties and watch them
    watchAttacheeProperties();
}

ItemStyleAttachedPrivate::~ItemStyleAttachedPrivate()
{
    // remove name from the theming engine
    if (!styleId.isEmpty())
        ThemeEnginePrivate::registerName(attachee, QString());
    // delete and style must be deleted delayed
    resetDelegate();
    resetStyle();
    if (componentContext)
        delete componentContext;
    componentContext = 0;
}

/*!
 * \internal
 * Enumerates attachee properties and marks them all candidates for styling. Connects
 * each property's notify signal so we get notified when those are binded in QML, so we
 * won't alter their value.
 */
void ItemStyleAttachedPrivate::watchAttacheeProperties()
{
    Q_Q(ItemStyleAttached);
    // list of properties we omit i.e. don't care or don't allow to be styled
    QString omit(OMIT_STYLED_PROPERTIES);
    // enumerate properties and figure out which one has binding
    const QMetaObject *mo = attachee->metaObject();
    for (int i = 0; i < mo->propertyCount(); i++) {
        const QMetaProperty prop = mo->property(i);

        if (!prop.hasNotifySignal() || omit.contains(prop.name()))
            continue;
        // check if attachee property has already bindings, leave if it has
        QQmlProperty qmlProp(attachee, prop.name(), QQmlEngine::contextForObject(attachee));
        if (QQmlPropertyPrivate::binding(qmlProp)) {
            continue;
        }
        // connect property's notify signal to watch when it gets changed so we can stop watching it
        qmlProp.connectNotifySignal(q, SLOT(_q_attacheePropertyChanged()));

        // we cannot detect whether a signal is connected as isSignalConnected() is
        // a protected method of QObject, and we cannot access attachee's protected
        // functions
        watchedProperties.insert(i, true);
    }
}

/*!
 * \internal
 * Captures attachee property changes which are to be removed from stylable ones.
 */
void ItemStyleAttachedPrivate::_q_attacheePropertyChanged()
{
    Q_Q(ItemStyleAttached);

    const QMetaObject *mo = attachee->metaObject();
    QMetaMethod signal = mo->method(q->senderSignalIndex());
    QString property = QString(signal.name()).remove("Changed");

    // was the property change invoked by the style update, exit
    if (style && style->isUpdating(property))
        return;

    // ban property from being styled
    watchedProperties.insert(mo->indexOfProperty(property.toLatin1()), false);

    // unbind style from attachee
    if (style)
        style->unbindProperty(property);
}

bool ItemStyleAttachedPrivate::updateStyleSelector()
{
    Selector path;
    SelectorNode::Relationship relation = SelectorNode::Child;
    QQuickItem *parent = attachee->parentItem();
    ItemStyleAttached *parentStyle = 0;

    path << SelectorNode(QuickUtils::instance().className(attachee).toLower(), styleClass, styleId, SelectorNode::Descendant);

    while (parent) {
        parentStyle = ThemeEnginePrivate::attachedStyle(parent);
        if (!parentStyle)
            relation = SelectorNode::Descendant;
        else {
            path[0] = SelectorNode(path[0].type(), path[0].getClass(), path[0].id(), relation);
            path.prepend(
                        SelectorNode(QuickUtils::instance().className(parentStyle->d_ptr->attachee),
                                     parentStyle->d_ptr->styleClass,
                                     parentStyle->d_ptr->styleId,
                                     SelectorNode::Descendant)
                        );
            relation = SelectorNode::Child;
        }
        parent = parent->parentItem();
    }

    if (path != styleSelector) {
        styleSelector = path;
        // need to refresh the style rule(s)
        styleRule = ThemeEnginePrivate::styleRuleForPath(styleSelector);
        return true;
    }

    return false;
}

bool ItemStyleAttachedPrivate::updateStyle()
{
    bool result = false;
    // do not do anything till the component gets complete?
    if (delayApplyingStyle)
       return result;

    resetStyle();
    if (!customStyle) {
        // make sure we have a theme
        if (styleRule && styleRule->style) {
            QObject *obj = styleRule->style->create(componentContext);
            style = qobject_cast<UCStyle*>(obj);
            if (!style)
                delete obj;
            result = (style != 0);
        }
    } else
        result = true;

    // reparent also custom styles!
    if (result && style) {
        style->setParent(attachee);
        style->bindStyledItem(attachee, watchedProperties);
        style->bindDelegate(delegate, watchedProperties);
        componentContext->setContextProperty(styleProperty, style);
    }
    return result;
}

bool ItemStyleAttachedPrivate::updateDelegate()
{
    bool result = false;
    // do not do anything till the component gets complete?
    if (delayApplyingStyle)
       return result;

    // delete delegate as the function can be called from elsewhere than updateCurrentStyle
    resetDelegate();
    if (!customDelegate) {
        // make sure we have a theme
        if (styleRule && styleRule->delegate) {
            QObject *obj = styleRule->delegate->create(componentContext);
            delegate = qobject_cast<QQuickItem*>(obj);
            if (!delegate && obj) {
                qDebug() << "WRONG DELEGATE!!!";
                delete obj;
            }
            result = (delegate != 0);
        }
    } else
        result = true;

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
                if (childItem && (childItem != delegate))
                    childItem->setParentItem(contentItem);
            }
        }
        // setup property "bindings" towards delegate properties
        if (style)
            style->bindDelegate(delegate, watchedProperties);
    }
    return result;
}

/*!
  \internal
  Updates the style and delegate variables. The style update is forced
  when the item changes the style lookup from private to theme.
*/
void ItemStyleAttachedPrivate::updateCurrentStyle()
{
    // the order is: clean up delegate then style, then create style and then delegate
    // so that when delegate is built we already have the styles ready for that
    resetDelegate();
    resetStyle();
    bool styleUpdated = updateStyle();
    bool delegateUpdated = updateDelegate();
    if (styleUpdated || delegateUpdated) {
        Q_Q(ItemStyleAttached);
        Q_EMIT q->styleChanged();
    }
}

void ItemStyleAttachedPrivate::resetStyle()
{
    if (style && !customStyle) {
        // clear bindings, disconnect as properties may change before the style
        // is deleted
        style->unbindItem(delegate);
        style->unbindItem(attachee);
        style->setParent(0);
        style->deleteLater();
        style = 0;
    }
}

void ItemStyleAttachedPrivate::resetDelegate()
{
    if (delegate && !customDelegate) {
        // remove all bindings between style and delegate
        if (style)
            style->unbindItem(delegate);
        delegate->setParent(0);
        delegate->setParentItem(0);
        delegate->deleteLater();
        delegate = 0;
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
    if (ThemeEnginePrivate::registerName(attachee, id)) {
        styleId = id;
        attachee->setProperty("name", styleId);
    } else {
        ThemeEnginePrivate::setError(QString("Instance %1 already registered. Resetting instance for %2.")
                                     .arg(styleId)
                                     .arg(styleClass));
        styleId = QString();
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
        if (!connectedToEngine) {
            connectedToEngine = (bool)QObject::connect(ThemeEngine::instance(), SIGNAL(themeChanged()), q, SLOT(_q_refreshStyle()));
            updateStyleSelector();
        }
    } else {
        if (connectedToEngine)
            connectedToEngine = !QObject::disconnect(ThemeEngine::instance(), SIGNAL(themeChanged()), q, SLOT(_q_refreshStyle()));
        if (!connectedToEngine)
            styleRule = 0;
    }
}

void ItemStyleAttachedPrivate::updateStyledItem(QQuickItem *item)
{
    QList<QQuickItem*> children = item->findChildren<QQuickItem*>();
    Q_FOREACH(QQuickItem *child, children) {
        ItemStyleAttached *style = ThemeEnginePrivate::attachedStyle(child);
        if (style)
            style->d_ptr->_q_reapplyStyling(child->parentItem());
    }
}


/*!
  \internal
  Internal slot to update the style of an item when the theme is changed. The slot
  is connected to the engine's themeChanged() signal. The slot should not be used
  from other methods as it will cause performance problems.
  */
void ItemStyleAttachedPrivate::_q_refreshStyle()
{
    // no need to delay style applying any longer
    delayApplyingStyle = false;

    // ... but style refresh is needed as the old styles are dead
    styleRule = ThemeEnginePrivate::styleRuleForPath(styleSelector);

    updateCurrentStyle();
}

/*!
 * \internal
 * Reapply styling on parent change.
 */
void ItemStyleAttachedPrivate::_q_reapplyStyling(QQuickItem *parentItem)
{
    if (!parentItem)
        // the component is most likely used in a delegate or is being deleted
        return;

    if (updateStyleSelector() || delayApplyingStyle) {
        delayApplyingStyle = false;
        updateCurrentStyle();
    }

    // need to reapply styling on each child of the attachee!
    // this will cause performance issues!
    updateStyledItem(attachee);
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
    return d->styleId;
}
/*!
  Updates the name property.
  */
void ItemStyleAttached::setName(const QString &name)
{
    Q_D(ItemStyleAttached);
    if (d->styleId.compare(name, Qt::CaseInsensitive)) {
        if (d->registerName(name.toLower())) {
            d->listenThemeEngine();
            if (d->updateStyleSelector())
                d->updateCurrentStyle();
        }
    }
}

/*!
  \qmlproperty string ItemStyle::class
  This property holds the style class identifier used by the item.
  When the engine locates the style rule to be applied on the item, it takes
  the class and name properties. If none is specified, the meta class
  name will be used to search for the style. This must be taken into account both
  when defining themes and designing items and applications.
  */
/*!
  class property.
  */
QString ItemStyleAttached::styleClass()
{
    Q_D(ItemStyleAttached);
    return d->styleClass.replace('.', ' ');
}
/*!
  Sets the class property value.
  */
void ItemStyleAttached::setStyleClass(const QString &styleClass)
{
    Q_D(ItemStyleAttached);
    if (d->styleClass.compare(styleClass.trimmed(), Qt::CaseInsensitive)) {
        // replace spaces with dots
        d->styleClass = styleClass.toLower().trimmed().replace(' ', '.');
        d->listenThemeEngine();
        if (d->updateStyleSelector())
            d->updateCurrentStyle();
    }
}

/*!
 *\qmlproperty string ItemStyle::path
 * This property holds the style path applied on the attachee item.
 */
QString ItemStyleAttached::path() const
{
    Q_D(const ItemStyleAttached);
    return d->styleRule ?
                d->styleRule->selector().toString() :
                QString("(null)");
}

/*!
  \qmlproperty Style ItemStyle::style
  The property holds the object containing the style configuration properties. This can
  either be defined by a theme style rule or the private style. When set, the item will
  no longer use the theme defined style properties but the ones set. The property must be
  reset (set to null object) in order to use the theme defined styles.

  Modifying the property alone will only affect the styling. The delegate will be used
  from the theme unless specified explicitly. Therefore items can be used with custom
  styling and theme defined delegate, theme style and custom delegate or both theme defined/
  custom.
  */
/*!
  \internal
  Returns the object created out of the style used.
  */
UCStyle *ItemStyleAttached::style() const
{
    Q_D(const ItemStyleAttached);
    return d->style;
}


/*!
  \internal
  Sets/resets the style object for the item.
  */
void ItemStyleAttached::setStyle(UCStyle *style)
{
    Q_D(ItemStyleAttached);
    if (d->style != style) {
        // clear the previous style
        if (!d->customStyle && d->style) {
            d->style->deleteLater();
            d->style = 0;
        }
        d->customStyle = (style != 0);
        if (d->customStyle && d->customDelegate)
            d->styleRule = 0;
        d->style = style;
        d->listenThemeEngine();
        if (d->updateStyle())
            Q_EMIT styleChanged();
    }
}

/*!
  \qmlproperty Item ItemStyle::delegate
  The property holds the Item containing the visuals. This can either be defined
  by a theme or can be a private element. When set, the item will no longer use
  the theme defined visuals but the ones set. The property must be reset (set to
  null object) in order to use the theme defined visuals.

  Modifying the property alone will only affect the visuals. Styles can be still
  used from the theme, unless specified explicitly. Therefore custom visuals can
  be made so that are aware of the styling properties.
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
