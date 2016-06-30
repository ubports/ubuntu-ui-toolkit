/*
 * Copyright 2014 Canonical Ltd.
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

#ifndef UCACTIONCONTEXT_H
#define UCACTIONCONTEXT_H

#include <QtCore/QObject>
#include <QtQml/QQmlListProperty>
#include <QtQml/QQmlParserStatus>
#include <QtCore/QSet>
#include <QtQml>
#include "ubuntutoolkitglobal.h"

class QQuickItem;

namespace UbuntuToolkit {

class UCAction;
class UCActionContextAttached;
class UBUNTUTOOLKIT_EXPORT UCActionContext : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(QQmlListProperty<UT_PREPEND_NAMESPACE(UCAction)> actions READ actions)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_CLASSINFO("DefaultProperty", "actions")
public:
    explicit UCActionContext(QObject *parent = 0);
    ~UCActionContext();

    static UCActionContextAttached *qmlAttachedProperties(QObject *owner);

    void classBegin() override;
    void componentComplete() override;
    void markActionsPublished(bool mark);
    bool isPopup() const
    {
        return m_popup;
    }

    QQmlListProperty<UCAction> actions();

    bool active();
    void setActive(bool active);
    void setEffectiveActive(bool active);

Q_SIGNALS:
    void activeChanged();

public Q_SLOTS:
    void addAction(UT_PREPEND_NAMESPACE(UCAction) *action);
    void removeAction(UT_PREPEND_NAMESPACE(UCAction) *action);

protected:
    QSet<UCAction*> m_actions;
    bool m_active:1;
    bool m_effectiveActive:1;
    // declare popup flag within ActionContext to avoid unnecessary object-casting
    // to detect whether a context is a popup or normal context.
    bool m_popup:1;
    friend class UCActionManager;

    static void append(QQmlListProperty<UCAction> *list, UCAction *action);
    static void clear(QQmlListProperty<UCAction> *list);
    static int count(QQmlListProperty<UCAction> *list);
};

class UBUNTUTOOLKIT_EXPORT UCPopupContext : public UCActionContext
{
    Q_OBJECT
public:
    explicit UCPopupContext(QObject *parent = 0);
};

class UBUNTUTOOLKIT_EXPORT UCActionContextAttached : public QObject
{
    Q_OBJECT
public:
    explicit UCActionContextAttached(QObject *owner);

    inline QQuickItem *owner() const
    {
        return m_owner;
    }
    inline UCActionContext *context() const
    {
        return m_context;
    }

private:
    QQuickItem *m_owner;
    UCActionContext *m_context;
    friend class UCActionContext;
};

UT_NAMESPACE_END

QML_DECLARE_TYPE(UT_PREPEND_NAMESPACE(UCActionContext))
QML_DECLARE_TYPEINFO(UT_PREPEND_NAMESPACE(UCActionContext), QML_HAS_ATTACHED_PROPERTIES)

#endif // UCACTIONCONTEXT_H
