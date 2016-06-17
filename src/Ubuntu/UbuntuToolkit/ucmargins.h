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
#include <ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

class UBUNTUTOOLKIT_EXPORT UCMargins : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal left MEMBER m_left NOTIFY leftChanged FINAL)
    Q_PROPERTY(qreal top MEMBER m_top NOTIFY topChanged FINAL)
    Q_PROPERTY(qreal right MEMBER m_right NOTIFY rightChanged FINAL)
    Q_PROPERTY(qreal bottom MEMBER m_bottom NOTIFY bottomChanged FINAL)
    Q_PROPERTY(qreal all MEMBER m_all NOTIFY allChanged FINAL)
public:
    UCMargins(QObject *parent = 0);

    qreal left() const;
    qreal top() const;
    qreal right() const;
    qreal bottom() const;

Q_SIGNALS:
    void leftChanged();
    void topChanged();
    void rightChanged();
    void bottomChanged();
    void allChanged();

private:
    qreal m_left = 0.0;
    qreal m_top = 0.0;
    qreal m_right = 0.0;
    qreal m_bottom = 0.0;
    qreal m_all = 0.0;
};

inline UCMargins::UCMargins(QObject *parent) : QObject(parent) {}

inline qreal UCMargins::left() const
{
    return qFuzzyIsNull(m_left) ? m_all : m_left;
}

inline qreal UCMargins::top() const
{
    return qFuzzyIsNull(m_top) ? m_all : m_top;
}

inline qreal UCMargins::right() const
{
    return qFuzzyIsNull(m_right) ? m_all : m_right;
}

inline qreal UCMargins::bottom() const
{
    return qFuzzyIsNull(m_bottom) ? m_all : m_bottom;
}

UT_NAMESPACE_END

#endif // UCMARGINS_H
