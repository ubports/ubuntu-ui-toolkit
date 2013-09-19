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

class StateSaverBackend : public QObject
{
    Q_OBJECT
public:
    ~StateSaverBackend();

    static StateSaverBackend &instance() {
        static StateSaverBackend instance;
        return instance;
    }

    static bool registerId(const QString &id);
    static void removeId(const QString &id);

    int load(const QString &id, QObject *item, const QStringList &properties);
    int save(const QString &id, QObject *item, const QStringList &properties);
    bool reset();

Q_SIGNALS:
    void deactivated();
    void idle();

protected:
    explicit StateSaverBackend(QObject *parent = 0);
    bool eventFilter(QObject *, QEvent *);
    void resetRegister(); // for testing purposes only

private Q_SLOTS:
    void initialize();

private:
    QPointer<QSettings> m_archive;
    QSet<QString> m_register;
    QTimer m_timer;
};

#endif // STATESAVERBACKEND_P_H
