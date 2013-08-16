#ifndef UCALARMSMODEL_H
#define UCALARMSMODEL_H

#include <QtCore/QAbstractListModel>
#include "ucalarm.h"

class UCAlarm;
class UCAlarmModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ count NOTIFY countChanged)
public:
    explicit UCAlarmModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

    QHash<int, QByteArray> roleNames() const;

    // invokables
    Q_INVOKABLE UCAlarm *get(int index);

    // property getters
    int count() const;
    
Q_SIGNALS:
    void countChanged();
    
private Q_SLOTS:
    void refresh();

private:
    UCAlarm m_alarm;
};

#endif // UCALARMSMODEL_H
