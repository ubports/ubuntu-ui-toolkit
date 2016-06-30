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

namespace UbuntuToolkit {

class UCAction;
class UCActionItemPrivate;
class UBUNTUTOOLKIT_EXPORT UCActionItem : public UCStyledItemBase
{
    Q_OBJECT
#ifndef Q_QDOC
    Q_PROPERTY(UT_PREPEND_NAMESPACE(UCAction) *action READ action WRITE setAction NOTIFY actionChanged FINAL)
#else
    Q_PROPERTY(UCAction *action READ action WRITE setAction NOTIFY actionChanged FINAL)
#endif
    Q_PROPERTY(QString text READ text WRITE setText RESET resetText NOTIFY textChanged)
    Q_PROPERTY(QUrl iconSource READ iconSource WRITE setIconSource RESET resetIconSource NOTIFY iconSourceChanged)
    Q_PROPERTY(QString iconName READ iconName WRITE setIconName RESET resetIconName NOTIFY iconNameChanged)

    // overrides
    Q_PROPERTY(bool enabled READ isEnabled WRITE setEnabled2 NOTIFY enabledChanged2)
    Q_PROPERTY(bool visible READ isVisible WRITE setVisible2 NOTIFY visibleChanged2 FINAL)
public:
    explicit UCActionItem(QQuickItem *parent = 0);

    UCAction *action() const;
    void setAction(UCAction *action);
    QString text();
    void setText(const QString &text);
    void resetText();
    QUrl iconSource();
    void setIconSource(const QUrl &iconSource);
    void resetIconSource();
    QString iconName();
    void setIconName(const QString &iconName);
    void resetIconName();

    void setVisible2(bool visible);
    void setEnabled2(bool enabled);

Q_SIGNALS:
    void actionChanged();
    void textChanged();
    void iconSourceChanged();
    void iconNameChanged();
    void triggered(const QVariant &value);

    void enabledChanged2();
    void visibleChanged2();

public Q_SLOTS:
    void trigger(const QVariant &value = QVariant());

protected:
    UCActionItem(UCActionItemPrivate &, QQuickItem *parent);

    Q_DECLARE_PRIVATE(UCActionItem)
    Q_PRIVATE_SLOT(d_func(), void _q_visibleBinding())
    Q_PRIVATE_SLOT(d_func(), void _q_enabledBinding())
    Q_PRIVATE_SLOT(d_func(), void _q_invokeActionTrigger(const QVariant &value))
};

UT_NAMESPACE_END

#endif // UCACTIONITEM_H
