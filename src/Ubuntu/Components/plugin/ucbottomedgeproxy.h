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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCBOTTOMEDGEPROXY_H
#define UCBOTTOMEDGEPROXY_H

#include <QtQuick/QQuickItem>

class UCBottomEdgeProxy : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(Status)

    Q_PROPERTY(QString text MEMBER m_text NOTIFY textChanged FINAL)
    Q_PROPERTY(QString iconName MEMBER m_iconName NOTIFY iconNameChanged FINAL)
    Q_PROPERTY(QUrl iconSource MEMBER m_iconSource NOTIFY iconSourceChanged FINAL)
    Q_PROPERTY(Status status MEMBER m_status NOTIFY statusChanged FINAL)
public:
    enum Status {
        Idle,
        Faded,
        Hinted,
        Revealed,
        CanCommit,
        Committed
    };
    explicit UCBottomEdgeProxy(QQuickItem *parent = 0);
    virtual ~UCBottomEdgeProxy();

Q_SIGNALS:
    void textChanged();
    void iconNameChanged();
    void iconSourceChanged();
    void statusChanged();

public Q_SLOTS:

protected:
    void classBegin();
    void itemChange(ItemChange change, const ItemChangeData &data);

    QString m_text;
    QString m_iconName;
    QUrl m_iconSource;
    Status m_status;
};

#endif // UCBOTTOMEDGEPROXY_H
