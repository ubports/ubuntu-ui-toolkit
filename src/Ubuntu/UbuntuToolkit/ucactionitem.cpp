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
 */

#include "ucactionitem.h"
#include "ucactionitem_p.h"
#include "ucaction.h"
#include "ucstyleditembase_p.h"
#define foreach Q_FOREACH
#include <QtQml/private/qqmlbinding_p.h>
#undef foreach

UT_NAMESPACE_BEGIN

UCActionItemPrivate::UCActionItemPrivate()
    : action(Q_NULLPTR)
    , flags(0)
{
}

void UCActionItemPrivate::init()
{
    Q_Q(UCActionItem);
    QObject::connect(q, &UCActionItem::enabledChanged, q, &UCActionItem::enabledChanged2);
    QObject::connect(q, &UCActionItem::visibleChanged, q, &UCActionItem::visibleChanged2);
}

/*!
 * \qmltype ActionItem
 * \instantiates UbuntuToolkit::UCActionItem
 * \inqmlmodule Ubuntu.Components
 * \ingroup ubuntu
 * \brief A visual representation of an Action. The API of ActionItem is a
 * copy of the API of \l Action, with additional properties to define
 * visual aspects of the ActionItem.
 *
 * If \l action is set, the values of the other properties will by default
 * be identical to the \l Action's property values. Setting the other properties
 * will override the properties copied from the \l Action.
 */

/*!
 * \qmlsignal ActionItem::triggered(var value)
 * Called when the actionItem is triggered.
 */
UCActionItem::UCActionItem(QQuickItem *parent)
    : UCStyledItemBase(*(new UCActionItemPrivate), parent)
{
    d_func()->init();
}

UCActionItem::UCActionItem(UCActionItemPrivate &dd, QQuickItem *parent)
    : UCStyledItemBase(dd, parent)
{
    d_func()->init();
}

bool UCActionItemPrivate::hasBindingOnProperty(const QString &name)
{
    Q_Q(UCActionItem);
    QQmlProperty property(q, name, qmlContext(q));
    return QQmlPropertyPrivate::binding(property) != Q_NULLPTR;
}

// update visible property
void UCActionItemPrivate::_q_visibleBinding()
{
    if (flags & CustomVisible) {
        return;
    }
    if (hasBindingOnProperty(QStringLiteral("visible"))) {
        flags |= CustomEnabled;
        return;
    }
    bool visible = action ? action->m_visible : true;
    q_func()->setVisible(visible);
}

// update enabled property
void UCActionItemPrivate::_q_enabledBinding()
{
    if (flags & CustomEnabled) {
        return;
    }
    if (hasBindingOnProperty(QStringLiteral("enabled"))) {
        flags |= CustomEnabled;
        return;
    }
    bool enabled = action ? action->m_enabled : true;
    q_func()->setEnabled(enabled);
}

// invoke actions' overridden triger() function
void UCActionItemPrivate::_q_invokeActionTrigger(const QVariant &value)
{
    invokeTrigger<UCAction>(action, value);
}

// setter called when bindings from QML set the value. Internal functions will
// all use the setVisible setter, so initialization and (re)parenting related
// visible alteration won't set the custom flag
void UCActionItem::setVisible2(bool visible)
{
    // set the custom flag and forward the value to the original proepry setter
    d_func()->flags |= UCActionItemPrivate::CustomVisible;
    setVisible(visible);
}
void UCActionItem::setEnabled2(bool enabled)
{
    d_func()->flags |= UCActionItemPrivate::CustomEnabled;
    setEnabled(enabled);
}

void UCActionItemPrivate::updateProperties()
{
    Q_Q(UCActionItem);
    if (!(flags & CustomText)) {
        Q_EMIT q->textChanged();
    }
    if (!(flags & CustomIconSource)) {
        Q_EMIT q->iconSourceChanged();
    }
    if (!(flags & CustomIconName)) {
        Q_EMIT q->iconNameChanged();
    }
}

void UCActionItemPrivate::attachAction(bool attach)
{
    Q_Q(UCActionItem);
    if (attach) {
        action->addOwningItem(q);
        QObject::connect(q, SIGNAL(triggered(QVariant)),
                q, SLOT(_q_invokeActionTrigger(QVariant)), Qt::DirectConnection);
        if (!(flags & CustomVisible)) {
            QObject::connect(action, SIGNAL(visibleChanged()),
                    q, SLOT(_q_visibleBinding()), Qt::DirectConnection);
        }
        if (!(flags & CustomEnabled)) {
            QObject::connect(action, SIGNAL(enabledChanged()),
                    q, SLOT(_q_enabledBinding()), Qt::DirectConnection);
        }
        if (!(flags & CustomText)) {
            QObject::connect(action, &UCAction::textChanged,
                    q, &UCActionItem::textChanged, Qt::DirectConnection);
        }
        if (!(flags & CustomIconSource)) {
            QObject::connect(action, &UCAction::iconSourceChanged,
                    q, &UCActionItem::iconSourceChanged, Qt::DirectConnection);
        }
        if (!(flags & CustomIconName)) {
            QObject::connect(action, &UCAction::iconNameChanged,
                    q, &UCActionItem::iconNameChanged, Qt::DirectConnection);
        }
    } else {
        action->removeOwningItem(q);
        QObject::disconnect(q, SIGNAL(triggered(QVariant)),
                   q, SLOT(_q_invokeActionTrigger(QVariant)));
        if (!(flags & CustomVisible)) {
            QObject::disconnect(action, SIGNAL(visibleChanged()),
                       q, SLOT(_q_visibleBinding()));
        }
        if (!(flags & CustomEnabled)) {
            QObject::disconnect(action, SIGNAL(enabledChanged()),
                       q, SLOT(_q_enabledBinding()));
        }
        if (!(flags & CustomText)) {
            QObject::disconnect(action, &UCAction::textChanged,
                       q, &UCActionItem::textChanged);
        }
        if (!(flags & CustomIconSource)) {
            QObject::disconnect(action, &UCAction::iconSourceChanged,
                       q, &UCActionItem::iconSourceChanged);
        }
        if (!(flags & CustomIconName)) {
            QObject::disconnect(action, &UCAction::iconNameChanged,
                       q, &UCActionItem::iconNameChanged);
        }
    }
}

/*!
 * \qmlproperty Action ActionItem::action
 * The \l Action associated with this ActionItem. If action is set, the values
 * of the \l Action properties are copied to the values of the ActionItem
 * properties, unless those were previously overridden.
 */
UCAction* UCActionItem::action() const
{
    Q_D(const UCActionItem);
    return d->action;
}
void UCActionItem::setAction(UCAction *action)
{
    Q_D(UCActionItem);
    if (d->action == action) {
        return;
    }
    if (d->action) {
        d->attachAction(false);
    }
    d->action = action;
    Q_EMIT actionChanged();

    if (d->action) {
        d->attachAction(true);
    }
    d->_q_visibleBinding();
    d->_q_enabledBinding();
    d->updateProperties();
}

/*!
 * \qmlproperty string ActionItem::text
 * The title of the actionItem. Defaults to the \c action.text.
 */
QString UCActionItem::text()
{
    Q_D(UCActionItem);
    if (d->flags & UCActionItemPrivate::CustomText) {
        return d->text;
    }
    return d->action ? d->action->text() : QString();
}
void UCActionItem::setText(const QString &text)
{
    Q_D(UCActionItem);

    if (d->action && !(d->flags & UCActionItemPrivate::CustomText)) {
        // disconnect change signal from Action
        disconnect(d->action, &UCAction::textChanged,
                   this, &UCActionItem::textChanged);
    }
    d->flags |= UCActionItemPrivate::CustomText;

    if (d->text == text) {
        return;
    }
    d->text = text;
    Q_EMIT textChanged();
}
void UCActionItem::resetText()
{
    Q_D(UCActionItem);
    d->text.clear();
    d->flags &= ~UCActionItemPrivate::CustomText;
    if (d->action) {
        // re-connect change signal from Action
        connect(d->action, &UCAction::textChanged,
                this, &UCActionItem::textChanged, Qt::DirectConnection);
    }
    Q_EMIT textChanged();
}

/*!
 * \qmlproperty url ActionItem::iconSource
 * The image associated with the ActionItem. Defaults to \c action.iconSource.
 *
 * This is the URL of any image file. If both iconSource and \l iconName are defined,
 * \l iconName will be ignored.
 */
QUrl UCActionItem::iconSource()
{
    Q_D(UCActionItem);
    if (d->flags & UCActionItemPrivate::CustomIconSource) {
        return d->iconSource;
    }
    if (d->action) {
        return d->action->m_iconSource;
    }
    return !iconName().isEmpty() ? QUrl(QString("image://theme/%1").arg(iconName())) : QUrl();
}
void UCActionItem::setIconSource(const QUrl &iconSource)
{
    Q_D(UCActionItem);

    if (d->action && !(d->flags & UCActionItemPrivate::CustomIconSource)) {
        // disconnect change signal from Action
        disconnect(d->action, &UCAction::iconSourceChanged,
                   this, &UCActionItem::iconSourceChanged);
    }
    d->flags |= UCActionItemPrivate::CustomIconSource;

    if (d->iconSource == iconSource) {
        return;
    }
    d->iconSource = iconSource;
    Q_EMIT iconSourceChanged();
}
void UCActionItem::resetIconSource()
{
    Q_D(UCActionItem);
    d->iconSource.clear();
    d->flags &= ~UCActionItemPrivate::CustomIconSource;
    if (d->action) {
        // re-connect change signal from Action
        connect(d->action, &UCAction::iconSourceChanged,
                this, &UCActionItem::iconSourceChanged, Qt::DirectConnection);
    }
    Q_EMIT iconSourceChanged();
}

/*!
 * \qmlproperty string ActionItem::iconName
 * The icon associated with the actionItem in the suru icon theme. Defaults to
 * \c action.iconName.
 *
 * \note The complete list of icons available in Ubuntu is not published yet.
 * For now please refer to the folders where the icon themes are installed:
 * \list
 *  \li Ubuntu Touch: \l file:/usr/share/icons/suru
 *  \li Ubuntu Desktop: \l file:/usr/share/icons/ubuntu-mono-dark
 * \endlist
 * These 2 separate icon themes will be merged soon.
 *
 * If both \l iconSource and iconName are defined, iconName will be ignored.
 */
QString UCActionItem::iconName()
{
    Q_D(UCActionItem);
    if (d->flags & UCActionItemPrivate::CustomIconName) {
        return d->iconName;
    }
    return d->action ? d->action->m_iconName : QString();
}
void UCActionItem::setIconName(const QString &iconName)
{
    Q_D(UCActionItem);

    if (d->action && !(d->flags & UCActionItemPrivate::CustomIconName)) {
        // disconnect change signal from Action
        disconnect(d->action, &UCAction::iconNameChanged,
                   this, &UCActionItem::iconNameChanged);
    }
    d->flags |= UCActionItemPrivate::CustomIconName;

    if (d->iconName == iconName) {
        return;
    }
    d->iconName = iconName;
    Q_EMIT iconNameChanged();
    // also sync iconSource if that is not a custom one or taken from action
    if (!d->action || (d->flags & UCActionItemPrivate::CustomIconSource)) {
        Q_EMIT iconSourceChanged();
    }
}
void UCActionItem::resetIconName()
{
    Q_D(UCActionItem);
    d->iconName.clear();
    d->flags &= ~UCActionItemPrivate::CustomIconName;
    if (d->action) {
        // re-connect change signal from Action
        connect(d->action, &UCAction::iconNameChanged,
                this, &UCActionItem::iconNameChanged, Qt::DirectConnection);
    }
    Q_EMIT iconNameChanged();
}

/*!
 * \qmlmethod void ActionItem::trigger(var value)
 * Trigger this action item if it is enabled.
 */
void UCActionItem::trigger(const QVariant &value)
{
    if (isEnabled()) {
        Q_EMIT triggered(value);
    }
}

UT_NAMESPACE_END

#include "moc_ucactionitem.cpp"
