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
 *
 * Author: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef UCSTYLEHINTS_H
#define UCSTYLEHINTS_H

#include <QtCore/QObject>
#define foreach Q_FOREACH
#include <QtQml/private/qqmlcustomparser_p.h>
#include <private/qv4engine_p.h>
#include <private/qpodvector_p.h>
#undef foreach
#include <QtQml/private/qqmlcompiler_p.h>

class UCStyleHintsParser;
class UCStyledItemBase;
class QQuickItem;
class PropertyChange;
class UCStyleHints : public QObject, public QQmlParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QQmlParserStatus)
    Q_PROPERTY(bool ignoreUnknownProperties MEMBER m_ignoreUnknownProperties)

public:
    explicit UCStyleHints(QObject *parent = 0);
    ~UCStyleHints();

private Q_SLOTS:
    void _q_applyStyleHints();

protected:
    void classBegin();
    void componentComplete();

private:
    class Expression {
    public:
        Expression(const QString &name, QQmlBinding::Identifier id, const QString& expr,
                         const QUrl &url, int line, int column)
            : name(name), id(id), expression(expr), url(url), line(line), column(column) {}
        QString name;
        QQmlBinding::Identifier id;
        QString expression;
        QUrl url;
        int line;
        int column;
    };

    bool m_decoded:1;
    bool m_completed:1;
    bool m_ignoreUnknownProperties:1;
    QPointer<UCStyledItemBase> m_styledItem;
    QList<Expression> m_expressions;
    QList< QPair<QString, QVariant> > m_values;
    QList< PropertyChange* > m_propertyBackup;
    QQmlRefPointer<QQmlCompiledData> m_cdata;

    friend class UCStyleHintsParser;

    void propertyNotFound(const QString &styleName, const QString &property);
    void decodeBinding(const QString &propertyPrefix, const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding);
};

class UCStyleHintsParser : public QQmlCustomParser
{
public:
    UCStyleHintsParser() : QQmlCustomParser(QQmlCustomParser::AcceptsSignalHandlers) {}

    virtual void verifyBindings(const QV4::CompiledData::Unit *qmlUnit, const QList<const QV4::CompiledData::Binding *> &bindings);
    virtual void applyBindings(QObject *obj, QQmlCompiledData *cdata, const QList<const QV4::CompiledData::Binding *> &bindings);

private:
    void verifyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding);
};

#endif // UCSTYLEHINTS_H
