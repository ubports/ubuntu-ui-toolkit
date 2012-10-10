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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef STYLE_H
#define STYLE_H

#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtQuick/QQuickItem>
#include <QtQml/QQmlParserStatus>

class StyleRulePrivate;
class QQmlComponent;
class QQmlEngine;
class StyleRule : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)

    Q_PROPERTY(QString selector READ selector WRITE setSelector)
    Q_PROPERTY(QQmlComponent *style READ style WRITE setStyle)
    Q_PROPERTY(QQmlComponent *delegate READ delegate WRITE setDelegate)

public:
    StyleRule(QObject *parent = 0);
    StyleRule(QQmlEngine *engine, const QString &selector, const QString &styleRule, const QString &delegateRule, QObject *parent = 0);
    ~StyleRule();

    void classBegin();
    void componentComplete();

    //create objects from style
    QObject *createStyle(QQmlContext *context);
    QQuickItem *createDelegate(QQmlContext *context);

    //internal

Q_SIGNALS:
    // the signal is emitted once the rule is completed
    void ruleChanged();

public Q_SLOTS:

public: //getters
    QString selector() const;
    QQmlComponent *style();
    QQmlComponent *delegate();
private: //setters
    void setSelector(const QString &selector);
    void setStyle(QQmlComponent *style);
    void setDelegate(QQmlComponent *delegate);

private:
    Q_DISABLE_COPY(StyleRule)
    Q_DECLARE_PRIVATE(StyleRule)
    QScopedPointer<StyleRulePrivate> d_ptr;
    Q_PRIVATE_SLOT(d_func(), void _q_componentCompleted(QQmlComponent::Status))
};

#endif // STYLE_H
