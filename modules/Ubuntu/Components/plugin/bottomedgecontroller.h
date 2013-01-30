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
    Q_PROPERTY(double currentHeight READ currentHeight WRITE setCurrentHeight NOTIFY currentHeightChanged)
    Q_PROPERTY(double targetHeight READ targetHeight WRITE setTargetHeight NOTIFY targetHeightChanged)
    Q_PROPERTY(bool forceHidden READ forceHidden WRITE setForceHidden NOTIFY forceHiddenChanged)

    Q_CLASSINFO("D-Bus Interface", "com.canonical.Shell.BottomEdgeController")
public:
    static BottomEdgeController& instance() {
        static BottomEdgeController instance;
        return instance;
    }

    double currentHeight() const;
    void setCurrentHeight(double currentHeight);

    double targetHeight() const;
    void setTargetHeight(double targetHeight);

    bool forceHidden() const;
    void setForceHidden(bool forceHidden);

Q_SIGNALS:
    void currentHeightChanged(double currentHeight);
    void targetHeightChanged(double targetHeight);
    void forceHiddenChanged(bool forceHidden);

private:
    BottomEdgeController();

    double m_currentHeight;
    double m_targetHeight;
    bool m_forceHidden;
};

#endif
