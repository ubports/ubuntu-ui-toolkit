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

#ifndef UCMARGINS_H
#define UCMARGINS_H

#include <QtCore/QObject>
#include <QtQuick/QQuickItem>

class UCMargins : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal left READ left WRITE setLeft NOTIFY leftChanged FINAL)
    Q_PROPERTY(qreal top READ top WRITE setTop NOTIFY topChanged FINAL)
    Q_PROPERTY(qreal right READ right WRITE setRight NOTIFY rightChanged FINAL)
    Q_PROPERTY(qreal bottom READ bottom WRITE setBottom NOTIFY bottomChanged FINAL)
public:
    explicit UCMargins(QObject *parent = 0)
        : QObject(parent)
    {
    }

    qreal left() const
    {
        return m_left;
    }
    void setLeft(qreal offset) {
        if (m_left != offset) {
            m_left = offset;
            Q_EMIT leftChanged();
        }
    }
    qreal top() const
    {
        return m_top;
    }
    void setTop(qreal offset) {
        if (m_top != offset) {
            m_top = offset;
            Q_EMIT topChanged();
        }
    }
    qreal right() const
    {
        return m_right;
    }
    void setRight(qreal offset) {
        if (m_right != offset) {
            m_right = offset;
            Q_EMIT rightChanged();
        }
    }
    qreal bottom() const
    {
        return m_bottom;
    }
    void setBottom(qreal offset) {
        if (m_bottom != offset) {
            m_bottom = offset;
            Q_EMIT bottomChanged();
        }
    }

Q_SIGNALS:
    void leftChanged();
    void topChanged();
    void rightChanged();
    void bottomChanged();

private:
    qreal m_left = 0.0;
    qreal m_top = 0.0;
    qreal m_right = 0.0;
    qreal m_bottom = 0.0;
};


#endif // UCMARGINS_H
