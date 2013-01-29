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
 */

#ifndef BOTTOMEDGECONTROLLER_H
#define BOTTOMEDGECONTROLLER_H

#include <QObject>

class BottomEdgeController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(double y READ y WRITE setY NOTIFY yChanged)
    Q_PROPERTY(double height READ height WRITE setHeight NOTIFY heightChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool locked READ locked WRITE setLocked NOTIFY lockedChanged)
    Q_PROPERTY(bool available READ available WRITE setAvailable NOTIFY availableChanged)

    Q_CLASSINFO("D-Bus Interface", "com.canonical.Shell.BottomEdgeController")
public:
    static BottomEdgeController& instance() {
        static BottomEdgeController instance;
        return instance;
    }

    double y() const;
    void setY(double y);

    double height() const;
    void setHeight(double height);

    bool active() const;
    void setActive(bool active);

    bool locked() const;
    void setLocked(bool locked);

    bool available() const;
    void setAvailable(bool available);

Q_SIGNALS:
    void yChanged(double y);
    void heightChanged(double height);
    void activeChanged(bool active);
    void lockedChanged(bool locked);
    void availableChanged(bool available);

private:
    BottomEdgeController();

    double m_y;
    double m_height;
    bool m_active;
    bool m_locked;
    bool m_available;
};

#endif
