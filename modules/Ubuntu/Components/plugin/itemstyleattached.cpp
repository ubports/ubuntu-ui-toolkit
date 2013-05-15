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
#include <private/qqmlcomponentattached_p.h>

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
    styleRule(0),
    completed(false),
    customStyle(false),
    customDelegate(false),
    connectedToEngine(false)
{
    styleClass = QuickUtils::instance().className(attachee).toLower();
    // refresh style upon reparenting!
    // there is no reason to do styling till the parent is not set and this applies
    // to the root objects too as even those have an internal parent
    QObject::connect(attachee, SIGNAL(parentChanged(QQuickItem*)), q_ptr, SLOT(_q_reapplyStyling(QQuickItem*)));

    // connect to one of the attached components to receive completion
    QQmlComponentAttached *component = QQmlComponent::qmlAttachedProperties(attachee);
    if (component) {
        QObject::connect(component, SIGNAL(completed()), q_ptr, SLOT(_q_refreshStyle()));
    } else {
        qmlInfo(q_ptr) << "WARNING: no attached component found for " << styleClass;
    }

    listenThemeEngine();

    //enum attachee properties and watch them
    watchAttacheeProperties();
}

ItemStyleAttachedPrivate::~ItemStyleAttachedPrivate()
{
    // remove name from the theming engine
    if (!styleId.isEmpty())
        ThemeEnginePrivate::registerName(attachee, QString());
    resetDelegate();
    resetStyle();
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
    // enumerate properties and figure out which one has binding
    const QMetaObject *mo = attachee->metaObject();
    QMetaMethod onAttacheePropertyChanged = q->metaObject()->method(q->metaObject()->indexOfSlot("_q_attacheePropertyChanged()"));
    for (int i = 0; i < mo->propertyCount(); i++) {
        const QMetaProperty prop = mo->property(i);

        if (!prop.hasNotifySignal() || UCStyle::omitProperty(prop.name())) {
            continue;
        }

        QQmlProperty qmlProp(attachee, prop.name(), QQmlEngine::contextForObject(attachee));
        QQmlAbstractBinding *binding = QQmlPropertyPrivate::binding(qmlProp);
        if (binding) {
            // mark as first time bound, so further styling can unbind it and do styling
            watchedProperties.mark(i, StyledPropertyMap::Bound, binding);
        } else {
            watchedProperties.mark(i, StyledPropertyMap::Enabled);
        }

        if (QLatin1String(prop.name()) == QLatin1String("font")) {
            // never ban the font property from being styled
            continue;
        }

        // connect property's notify signal to watch when it gets changed so we can stop watching it
        QObject::connect(attachee, prop.notifySignal(), q, onAttacheePropertyChanged);
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

    int index = mo->indexOfProperty(property.toLatin1());
    if (watchedProperties.isBanned(index))
        return;

    // ban property from being styled
    watchedProperties.mark(index, StyledPropertyMap::Banned);

    // unbind style from attachee
    if (style)
        style->unbindProperty(property);
}

bool ItemStyleAttachedPrivate::updateStyle()
{
    bool result = false;
    // do not do anything till the component gets complete?
    if (!completed)
       return result;

    if (!customStyle) {
        // make sure we have a theme
        if (styleRule && styleRule->style) {
            QQmlContext *context = new QQmlContext(qmlContext(attachee));
            context->setContextProperty(itemProperty, attachee);
            QObject *obj = styleRule->style->create(context);
            style = qobject_cast<UCStyle*>(obj);
            if (!style) {
                delete obj;
                delete context;
            } else {
                // set style as parent for the context so it gets deleted together with the style
                context->setParent(style);
                // set owner so we know that the style object has been created by theming
                style->setOwner(attachee);
            }
            result = (style != 0);
        }
    } else
        result = true;

    // reparent also custom styles!
    if (result && style) {
        style->bindItem(attachee, watchedProperties, true);
        style->bindItem(delegate, watchedProperties, false);
    }
    return result;
}

bool ItemStyleAttachedPrivate::updateDelegate()
{
    bool result = false;
    // do not do anything till the component gets complete?
    if (!completed)
       return result;

    if (!customDelegate) {
        // make sure we have a theme
        if (styleRule && styleRule->delegate) {
            QQmlContext * context = new QQmlContext(qmlContext(attachee));
            context->setContextProperty(itemProperty, attachee);
            delegate = qobject_cast<QQuickItem*>(styleRule->delegate->create(context));
            if (delegate) {
                // set delegate as parent for the context so it gets deleted together with the style
                context->setParent(delegate);
                // set delegate's parent to attachee
                delegate->setParent(attachee);
            } else {
                delete context;
            }
            result = (delegate != 0);
        }
    } else
        result = true;

    if (delegate && ((delegate->parent() != attachee) || (delegate->parentItem() != attachee))) {
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
            style->bindItem(delegate, watchedProperties, false);
    }
    return result;
}

/*!
  \internal
  Updates the style and delegate variables. The style update is forced
  when the item changes the style lookup from private to theme.
*/
int ItemStyleAttachedPrivate::updateTheme()
{
    int result = NoUpdate;
    // check if the new rule differs from the previous one
    StyleCache::StyleData *newRule = ThemeEnginePrivate::styleRuleForPath(styleSelector);
    if (newRule && (styleRule != newRule)) {
        // check what has been changed
        bool styleChanged = (!styleRule || (styleRule && (styleRule->style != newRule->style)));
        bool delegateChanged = (!styleRule || (styleRule && (styleRule->delegate != newRule->delegate)));
        styleRule = newRule;

        // the order is: clean up delegate then style, then create style and then delegate
        // so that when delegate is built we already have the styles ready for that
        if (delegateChanged) {
            resetDelegate();
        }
        if (styleChanged) {
            resetStyle();
            styleChanged = updateStyle();
        }
        if (delegateChanged) {
            delegateChanged = updateDelegate();
        } else if (style) {
            style->bindItem(delegate, watchedProperties, false);
        }
        if (styleChanged || delegateChanged) {
            if (styleChanged) {
                result |= StyleUpdated;
            }
            if (delegateChanged) {
                result |= DelegateUpdated;
            }
            Q_Q(ItemStyleAttached);
            Q_EMIT q->styleChanged();
        }
    }
    return result;
}

void ItemStyleAttachedPrivate::resetStyle()
{
    if (!style)
        return;
    // clear bindings, disconnect as properties may change before the style
    // is deleted
    style->unbindItem(delegate);
    style->unbindItem(attachee);

    // delete style also if there is an owner set to it
    if (!customStyle || style->owner()) {
        // reset style object before we delete it, themed animations may get changed
        // during the style deletion which will cause invalid pointer operations
        // in style bindings cleanup
        // https://bugs.launchpad.net/ubuntu-ui-toolkit/+bug/1175394
        QObject *object = style;
        style = 0;
        delete object;
    }
}

void ItemStyleAttachedPrivate::resetDelegate()
{
    if (!delegate)
        return;
    // remove all bindings between style and delegate
    if (style)
        style->unbindItem(delegate);

    // delete delegate also if there is an owner set to it
    if (!customDelegate || (delegate->parent() == attachee)) {
        delegate->setParent(0);
        delegate->setParentItem(0);
        delete delegate;
        delegate = 0;
    }
}

/*!
 * \internal
 * Applies styling on children recoursively.
 */
void ItemStyleAttachedPrivate::applyStyleOnChildren(QQuickItem *item)
{
    QList<QQuickItem*> children = item->childItems();
    Q_FOREACH(QQuickItem *child, children) {
        ItemStyleAttached *style = ThemeEnginePrivate::attachedStyle(child);
        if (style)
            style->d_ptr->_q_reapplyStyling(child->parentItem());
        else
            applyStyleOnChildren(child);
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
        }
    } else {
        if (connectedToEngine)
            connectedToEngine = !QObject::disconnect(ThemeEngine::instance(), SIGNAL(themeChanged()), q, SLOT(_q_refreshStyle()));
        if (!connectedToEngine) {
            styleRule = 0;
            styleSelector.clear();
        }
    }
}

/*!
 * \internal
 * Transfers the ownership of the style object (style or delegate) to teh current item.
 */
void ItemStyleAttachedPrivate::gainOwnershipOverStyleObject(QObject *styleObject, bool style)
{
    if (!styleObject)
        return;

    if (style) {
        UCStyle *style = qobject_cast<UCStyle*>(styleObject);
        if (!style)
            return;
        ItemStyleAttached *attached = ThemeEnginePrivate::attachedStyle(style->owner());
        if (attached) {
            attached->d_ptr->style = 0;
            style->setOwner(attachee);
        }
    } else {
        QQuickItem *delegate = qobject_cast<QQuickItem*>(styleObject);
        if (!delegate)
            return;
        // return if the delegate has no parent item set - case ItemStyle.delegate: Item{}
        if (!delegate->parentItem())
            return;
        ItemStyleAttached *attached = ThemeEnginePrivate::attachedStyle(delegate->parentItem());
        // check if the object is set as parent's delegate
        if (attached && (attached->d_ptr->delegate == delegate)) {
            // need to take ownership
            attached->d_ptr->delegate = 0;
            // set the new parent of the delegate
            delegate->setParent(attachee);
        }
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
    bool applyOnChildren = !completed;
    if (!completed) {
        styleSelector = Selector(attachee);
    }
    completed = true;

    // ... but style refresh is needed as the old styles are dead
    styleRule = 0;

    int update = updateTheme();
    if (applyOnChildren) {
        // theme applied first time, style on custom style objects
        if (((update & StyleUpdated) != StyleUpdated) && customStyle) {
            updateStyle();
        }
        if (((update & DelegateUpdated) != DelegateUpdated) && customDelegate) {
            updateDelegate();
        }
        // this will happen only upon the styled item gets completed
        applyStyleOnChildren(attachee);
    }
}

/*!
 * \internal
 * Reapply styling on parent change.
 */
void ItemStyleAttachedPrivate::_q_reapplyStyling(QQuickItem *parentItem)
{
    if (!parentItem || !completed)
        // the component is most likely used in a delegate, is being deleted
        // or not yet complete
        return;

    Selector newSelector(attachee);
    if (newSelector != styleSelector) {
        styleSelector = newSelector;
        updateTheme();
    }
    // need to reapply styling on each child of the attachee!
    // this will cause performance issues!
    applyStyleOnChildren(attachee);
}

/*==============================================================================
  */

ItemStyleAttached::ItemStyleAttached(QObject *parent) :
    QObject(parent),
    d_ptr(new ItemStyleAttachedPrivate(this, parent))
{
    d_ptr->styleSelector = Selector(d_ptr->attachee);
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
            d->styleSelector.update();
            if (!d->completed)
                return;
            d->updateTheme();
            // refresh children theme
            d->applyStyleOnChildren(d->attachee);
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
        d->styleSelector.update();
        if (!d->completed)
            return;
        d->updateTheme();
        // refresh children theme
        d->applyStyleOnChildren(d->attachee);
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
        d->resetStyle();
        // check if style belongs to a styled item, if it does, detach it
        d->customStyle = (style != 0);
        d->gainOwnershipOverStyleObject(style, true);
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
        d->resetDelegate();
        d->customDelegate = (delegate != 0);
        d->gainOwnershipOverStyleObject(delegate, false);
        d->delegate = delegate;
        d->listenThemeEngine();
        if (d->updateDelegate())
            Q_EMIT styleChanged();
    }
}

#include "moc_itemstyleattached.cpp"
