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
#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeParserStatus>

class StylePrivate;
class QDeclarativeComponent;
class Style : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QString styleClass READ styleClass WRITE setStyleClass NOTIFY styleChanged)
    Q_PROPERTY(QString instanceId READ instanceId WRITE setInstanceId NOTIFY styleChanged)
    Q_PROPERTY(QString selector READ selector WRITE setSelector NOTIFY styleChanged)
    Q_PROPERTY(QDeclarativeComponent *style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate NOTIFY styleChanged)
    Q_PROPERTY(QDeclarativeListProperty<QObject> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")
public:
    Style(QObject *parent = 0);
    ~Style();

    void classBegin();
    void componentComplete();

signals:
    void styleChanged();

public slots:

public: //getter/setters
    QString styleClass() const;
    void setStyleClass(const QString &styleClass);
    QString instanceId() const;
    void setInstanceId(const QString &instanceId);
    QString selector() const;
    void setSelector(const QString &selector);
    QDeclarativeComponent *style() const;
    void setStyle(QDeclarativeComponent *style);
    QDeclarativeComponent *delegate() const;
    void setDelegate(QDeclarativeComponent *delegate);
    QDeclarativeListProperty<QObject> data();

private:
    Q_DISABLE_COPY(Style)
    Q_DECLARE_PRIVATE(Style)
    friend class ThemeEngine;
    friend class StyledItem;
    QScopedPointer<StylePrivate> d_ptr;

};

QML_DECLARE_TYPE(Style)

#endif // STYLE_H
