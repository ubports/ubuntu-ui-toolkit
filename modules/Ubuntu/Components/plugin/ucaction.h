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

class QQmlComponent;
class UCAction : public QObject
{
    Q_OBJECT

    // transferred from Unity Actions
    Q_ENUMS(Type)
    Q_PROPERTY(QString name MEMBER m_name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged)
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
    Q_PROPERTY(QVariant shortcut MEMBER m_shortcut WRITE setShortcut NOTIFY shortcutChanged REVISION 1)
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

    inline bool isPublished() const
    {
        return m_published;
    }

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
    void shortcutChanged(const QVariant& shortcut);
    void triggered(const QVariant &value);

public Q_SLOTS:
    void trigger(const QVariant &value = QVariant());

private:
    bool m_factoryIconSource:1;
    bool m_enabled:1;
    bool m_visible:1;
    bool m_published:1;
    QQmlComponent *m_itemHint;
    QString m_name;
    QString m_text;
    QString m_iconName;
    QUrl m_iconSource;
    QString m_description;
    QString m_keywords;
    Type m_parameterType;
    QVariant m_shortcut;

    friend class UCActionContext;
    friend class UCListItemPrivate;
    friend class UCListItemAttached;
    friend class UCListItemActionsPrivate;

    bool isValidType(QVariant::Type valueType);
    void generateName();
    void setName(const QString &name);
    void setIconName(const QString &name);
    void setIconSource(const QUrl &url);
    void setItemHint(QQmlComponent *);
    void setShortcut(const QVariant&);
    bool event(QEvent *event);
};

#endif // UCACTION_H
