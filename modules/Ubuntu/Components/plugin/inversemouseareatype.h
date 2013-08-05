/*
 * Copyright 2012 Canonical Ltd.
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
#ifndef INVERSEMOUSEAREATYPE_H
#define INVERSEMOUSEAREATYPE_H

#include <QtQuick/QQuickItem>
#include <private/qquickmousearea_p.h>

//class QQuickMouseEvent;
//class QMouseEvent;
class InverseMouseAreaType : public QQuickMouseArea
{
    Q_OBJECT
    Q_PROPERTY(QQuickItem *sensingArea READ sensingArea WRITE setSensingArea NOTIFY sensingAreaChanged)
public:
    explicit InverseMouseAreaType(QQuickItem *parent = 0);
    ~InverseMouseAreaType();

    Q_INVOKABLE virtual bool contains(const QPointF &point) const;

protected:
    void componentComplete();

private: // getter/setter
    QQuickItem *sensingArea() const;
    void setSensingArea(QQuickItem *sensing);

Q_SIGNALS:
    void sensingAreaChanged();

private Q_SLOTS:
    void update();
    
private:
    bool m_ready;
    QQuickItem *m_sensingArea;
};

#endif // INVERSEMOUSEAREATYPE_H
