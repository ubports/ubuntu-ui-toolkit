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
#include <QStringList>
#include <qdeclarative.h>
#include <QtDeclarative/QDeclarativeItem>
#include <QtDeclarative/QDeclarativeParserStatus>

#include <QLibrary>

class StylePrivate; class QLibrary;
class QDeclarativeComponent;
class Style : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QString selector READ selector WRITE setSelector NOTIFY styleChanged)
    Q_PROPERTY(QDeclarativeComponent *style READ style WRITE setStyle NOTIFY styleChanged)
    Q_PROPERTY(QDeclarativeComponent *visuals READ visuals WRITE setVisuals NOTIFY styleChanged)
    Q_PROPERTY(QDeclarativeListProperty<Style> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")
    Q_FLAGS(StyleType StyleTypes)
public:
    enum StyleType {
        ConfigurationStyle = 0x01,
        VisualStyle = 0x02
    };
    Q_DECLARE_FLAGS(StyleTypes, StyleType)

    Style(QObject *parent = 0);
    ~Style();

    void classBegin();
    void componentComplete();

    StyleTypes styleType();

signals:
    void styleChanged();

public slots:

public: //getter/setters
    QString selector() const;
    void setSelector(const QString &selector);
    QDeclarativeComponent *style();
    void setStyle(QDeclarativeComponent *style);
    QDeclarativeComponent *visuals();
    void setVisuals(QDeclarativeComponent *visuals);
    QDeclarativeListProperty<Style> data();

private:
    Q_DISABLE_COPY(Style)
    Q_DECLARE_PRIVATE(Style)
    friend class ThemeEngine;
    friend class StyledItem;
    QScopedPointer<StylePrivate> d_ptr;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(Style::StyleTypes)


QML_DECLARE_TYPE(Style)

#endif // STYLE_H
