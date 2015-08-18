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
#ifndef UCACTIONITEM_H
#define UCACTIONITEM_H

#include "ucstyleditembase.h"

class UCAction;
class UCActionItem : public UCStyledItemBase
{
    Q_OBJECT
    Q_PROPERTY(UCAction *action MEMBER m_action WRITE setAction NOTIFY actionChanged FINAL)
    Q_PROPERTY(QString text READ text WRITE setText NOTIFY textChanged)
    Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName NOTIFY iconNameChanged)
public:
    explicit UCActionItem(QQuickItem *parent = 0);

    void setAction(UCAction *action);
    QString text();
    void setText(const QString &text);
    QUrl iconSource();
    void setIconSource(const QUrl &iconSource);
    QString iconName();
    void setIconName(const QString &iconName);

Q_SIGNALS:
    void actionChanged();
    void textChanged();
    void iconSourceChanged();
    void iconNameChanged();
    void triggered(const QVariant &value);

public Q_SLOTS:
    void trigger(const QVariant &value = QVariant());

protected Q_SLOTS:
    void _q_visibleChanged();
    void _q_enabledChanged();

protected:
    enum {
        CustomText = 0x01,
        CustomIconSource = 0x02,
        CustomIconName = 0x04,
        CustomVisible = 0x40,
        CustomEnabled = 0x80
    };
    QString m_text;
    QString m_iconName;
    QUrl m_iconSource;
    UCAction *m_action;
    quint8 m_flags;

};

#endif // UCACTIONITEM_H
