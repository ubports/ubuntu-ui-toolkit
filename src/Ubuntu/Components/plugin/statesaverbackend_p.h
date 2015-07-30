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

#ifndef STATESAVERBACKEND_P_H
#define STATESAVERBACKEND_P_H

#include <QtCore/QObject>
#include <QtCore/QSettings>
#include <QtCore/QPointer>
#include <QtCore/QSet>
#include <QtCore/QTimer>
#include <QtCore/QStack>

class StateSaverBackend : public QObject
{
    Q_OBJECT
public:
    ~StateSaverBackend();

    static StateSaverBackend &instance() {
        static StateSaverBackend instance;
        return instance;
    }

    bool enabled() const;
    void setEnabled(bool enabled);

    bool registerId(const QString &id);
    void removeId(const QString &id);

    int load(const QString &id, QObject *item, const QStringList &properties);
    int save(const QString &id, QObject *item, const QStringList &properties);

public Q_SLOTS:
    bool reset();

Q_SIGNALS:
    void enabledChanged(bool enabled);
    void initiateStateSaving();

protected:
    explicit StateSaverBackend(QObject *parent = 0);

private Q_SLOTS:
    void initialize();
    void cleanup();
    void signalHandler(int type);

private:
    QPointer<QSettings> m_archive;
    QSet<QString> m_register;
    QStack<QString> m_groupStack;
    bool m_globalEnabled;
};

#endif // STATESAVERBACKEND_P_H
