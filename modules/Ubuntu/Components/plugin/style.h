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

class StyleRulePrivate;
class QDeclarativeComponent;
class StyleRule : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)

    Q_PROPERTY(QString selector READ selector WRITE setSelector)
    Q_PROPERTY(QDeclarativeComponent *style READ style WRITE setStyle)
    Q_PROPERTY(QDeclarativeComponent *delegate READ delegate WRITE setDelegate)
/*
    Q_PROPERTY(QDeclarativeListProperty<QObject> data READ data)
    Q_CLASSINFO("DefaultProperty", "data")
*/
public:
    StyleRule(QObject *parent = 0);
    ~StyleRule();

    void classBegin();
    void componentComplete();

signals:
    // the signal is emitted once the rule is completed
    void ruleChanged();

public slots:

public: //getter/setters
    QString selector() const;
    void setSelector(const QString &selector);
    QDeclarativeComponent *style();
    void setStyle(QDeclarativeComponent *style);
    QDeclarativeComponent *delegate();
    void setDelegate(QDeclarativeComponent *delegate);
    QDeclarativeListProperty<QObject> data();

private:
    Q_DISABLE_COPY(StyleRule)
    Q_DECLARE_PRIVATE(StyleRule)
    friend class ThemeEngine;
    friend class StyledItem;
    QScopedPointer<StyleRulePrivate> d_ptr;
};

QML_DECLARE_TYPE(StyleRule)

#endif // STYLE_H
