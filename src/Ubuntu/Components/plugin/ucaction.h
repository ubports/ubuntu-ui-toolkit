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

#ifndef UCACTION_H
#define UCACTION_H

#include <QtCore/QObject>
#include <QtCore/QVariant>
#include <QtCore/QUrl>
#include <QtGui/QKeySequence>
#include <QtQml>
#include <QtQml/QQmlListProperty>
#include <QtQml/private/qpodvector_p.h>

// the function detects whether QML has an overridden trigger() slot available
// and invokes the one with the appropriate signature
template<class T>
inline void invokeTrigger(T *object, const QVariant &value)
{
    bool invoked = false;
    const QMetaObject *mo = object->metaObject();
    int offset = mo->methodOffset();
    int paramlessTriggerIndex = mo->indexOfSlot("trigger()") - offset;
    int paramTriggerIndex = mo->indexOfSlot("trigger(QVariant)") - offset;

    /* if we have the parametered version, call that even if the value given is invalid */
    if (paramTriggerIndex >= 0) {
        invoked = QMetaObject::invokeMethod(object, "trigger", Q_ARG(QVariant, value));
    } else if (paramlessTriggerIndex >= 0) {
        invoked = QMetaObject::invokeMethod(object, "trigger");
    }
    if (!invoked) {
        object->trigger(value);
    }
}

class QQmlComponent;
class QQuickItem;
class UCActionAttached;
class UCAction : public QObject
{
    Q_OBJECT

    // transferred from Unity Actions
    Q_ENUMS(Type)
    Q_PROPERTY(QString name MEMBER m_name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString text READ text WRITE setText RESET resetText NOTIFY textChanged)
    Q_PROPERTY(QString iconName MEMBER m_iconName WRITE setIconName NOTIFY iconNameChanged)
    Q_PROPERTY(QString description MEMBER m_description NOTIFY descriptionChanged)
    Q_PROPERTY(QString keywords MEMBER m_keywords NOTIFY keywordsChanged)
    Q_PROPERTY(bool enabled MEMBER m_enabled NOTIFY enabledChanged)
    Q_PROPERTY(Type parameterType MEMBER m_parameterType NOTIFY parameterTypeChanged)

    // Toolkit Actions API
    Q_PROPERTY(QUrl iconSource MEMBER m_iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(bool visible MEMBER m_visible NOTIFY visibleChanged)
    Q_PROPERTY(QQmlComponent *itemHint MEMBER m_itemHint WRITE setItemHint)

    // QtQuickControls.Action
    Q_PROPERTY(QVariant shortcut MEMBER m_shortcut WRITE setShortcut RESET resetShortcut NOTIFY shortcutChanged REVISION 1)
public:
    enum Type {
        None,
        String,
        Integer,
        Bool,
        Real,
        Object = 0xFF
    };

    explicit UCAction(QObject *parent = 0);
    ~UCAction();

    inline bool isPublished() const
    {
        return m_published;
    }
    inline bool isEnabled() const
    {
        return m_enabled;
    }
    inline QQuickItem *lastOwningItem() const
    {
        return m_owningItems.count() > 0 ?
                    m_owningItems.at(m_owningItems.count() - 1) : Q_NULLPTR;
    }
    void addOwningItem(QQuickItem *item);
    void removeOwningItem(QQuickItem *item);

    void setName(const QString &name);
    QString text();
    void setText(const QString &text);
    void resetText();
    void setIconName(const QString &name);
    void setIconSource(const QUrl &url);
    void setItemHint(QQmlComponent *);
    void setShortcut(const QVariant&);
    void resetShortcut();

Q_SIGNALS:
    void nameChanged();
    void textChanged();
    void iconNameChanged();
    void descriptionChanged();
    void keywordsChanged();
    void enabledChanged();
    void parameterTypeChanged();
    void iconSourceChanged();
    void visibleChanged();
    void shortcutChanged();
    void triggered(const QVariant &value);

public Q_SLOTS:
    void trigger(const QVariant &value = QVariant());

private:
    QPODVector<QQuickItem*, 4> m_owningItems;
    QString m_name;
    QString m_text;
    QString m_iconName;
    QUrl m_iconSource;
    QString m_description;
    QString m_keywords;
    QVariant m_shortcut;
    QKeySequence m_mnemonic;
    QQmlComponent *m_itemHint;
    Type m_parameterType;
    bool m_factoryIconSource:1;
    bool m_enabled:1;
    bool m_visible:1;
    bool m_published:1;

    friend class UCActionContext;
    friend class UCActionItem;
    friend class UCActionItemPrivate;
    friend class UCListItemPrivate;
    friend class UCListItemAttached;
    friend class UCListItemActionsPrivate;

    bool isValidType(QVariant::Type valueType);
    void generateName();
    void setMnemonicFromText(const QString &text);
    bool event(QEvent *event);
    void onKeyboardAttached();
};
QML_DECLARE_TYPE(UCAction)

#endif // UCACTION_H
