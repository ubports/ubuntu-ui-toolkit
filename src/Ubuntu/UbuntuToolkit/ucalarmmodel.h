/*
 * Copyright 2013 Canonical Ltd.
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

#ifndef UCALARMSMODEL_H
#define UCALARMSMODEL_H

#include <QtCore/QAbstractListModel>
#include <QtQml/QQmlParserStatus>
#include "ucalarm.h"

namespace UbuntuToolkit {

class UCAlarm;
class UBUNTUTOOLKIT_EXPORT UCAlarmModel : public QAbstractListModel, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit UCAlarmModel(QObject *parent = 0);
    ~UCAlarmModel();

    void classBegin() override;
    void componentComplete() override;

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    QHash<int, QByteArray> roleNames() const override;

    // invokables
    Q_INVOKABLE UT_PREPEND_NAMESPACE(UCAlarm) *get(int index);

    // property getters
    int count() const;
    
Q_SIGNALS:
    void countChanged();

public Q_SLOTS:
    Q_REVISION(1) void refresh();

private Q_SLOTS:
    void refreshStart();
    void refreshEnd();
    void update(int index);
    void removeStarted(int index);
    void removeFinished();
    void insertStarted(int index);
    void insertFinished();
    void moveStarted(int from, int to);
    void moveFinished();

private:
    bool m_moved:1;
};

UT_NAMESPACE_END

#endif // UCALARMSMODEL_H
