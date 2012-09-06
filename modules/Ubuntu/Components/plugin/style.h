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

#ifndef STYLE_H
#define STYLE_H

#include <QObject>
#include <qdeclarative.h>
#include <QStringList>
#include <QtDeclarative/QDeclarativeParserStatus>

class Style : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QStringList states READ states WRITE setStates NOTIFY statesChanged)
public:
    explicit Style(QObject *parent = 0);

    void classBegin();
    void componentComplete();

    // getter/setter is public so it can be accessed in ThemeEngine
    QStringList states() const;
    void setStates(const QStringList &states);

signals:
    void statesChanged();

public slots:

    QString styleClass() const;

private:
    QStringList m_states;
    QString m_styleClass;
};

QML_DECLARE_TYPE(Style)

#endif // STYLE_H
