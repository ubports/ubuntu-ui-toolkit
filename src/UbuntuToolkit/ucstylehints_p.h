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

#ifndef UCSTYLEHINTS_P_H
#define UCSTYLEHINTS_P_H

#include <QtCore/QObject>
#define foreach Q_FOREACH
#include <QtQml/private/qpodvector_p.h>
#include <QtQml/private/qqmlcustomparser_p.h>
#include <QtQml/private/qv4engine_p.h>
#undef foreach

#include <UbuntuToolkit/ubuntutoolkitglobal.h>

class QQuickItem;

UT_NAMESPACE_BEGIN

class UCStyleHintsParser;
class UCStyledItemBase;
class PropertyChange;
class UBUNTUTOOLKIT_EXPORT UCStyleHints : public QObject, public QQmlParserStatus
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
    void classBegin() override;
    void componentComplete() override;

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
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    QQmlRefPointer<QV4::ExecutableCompilationUnit> m_cdata;
#else
    QQmlRefPointer<QV4::CompiledData::CompilationUnit> m_cdata;
#endif

    friend class UCStyleHintsParser;

    void propertyNotFound(const QString &styleName, const QString &property);
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    void decodeBinding(const QString &propertyPrefix, const QQmlRefPointer<QV4::ExecutableCompilationUnit> &compilationUnit, const QV4::CompiledData::Binding *binding);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    void decodeBinding(const QString &propertyPrefix, const QQmlRefPointer<QV4::CompiledData::CompilationUnit> &compilationUnit, const QV4::CompiledData::Binding *binding);
#else
    void decodeBinding(const QString &propertyPrefix, const QV4::CompiledData::CompilationUnit *compilationUnit, const QV4::CompiledData::Binding *binding);
#endif
};

class UBUNTUTOOLKIT_EXPORT UCStyleHintsParser : public QQmlCustomParser
{
public:
    UCStyleHintsParser() : QQmlCustomParser(QQmlCustomParser::AcceptsSignalHandlers) {}

#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    void verifyBindings(const QQmlRefPointer<QV4::ExecutableCompilationUnit> &compilationUnit, const QList<const QV4::CompiledData::Binding *> &bindings) override;
    void applyBindings(QObject *obj, const QQmlRefPointer<QV4::ExecutableCompilationUnit> &compilationUnit, const QList<const QV4::CompiledData::Binding *> &bindings) override;
#elif QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    void verifyBindings(const QQmlRefPointer<QV4::CompiledData::CompilationUnit> &compilationUnit, const QList<const QV4::CompiledData::Binding *> &bindings) override;
    void applyBindings(QObject *obj, const QQmlRefPointer<QV4::CompiledData::CompilationUnit> &compilationUnit, const QList<const QV4::CompiledData::Binding *> &bindings) override;
#else
    void verifyBindings(const QV4::CompiledData::Unit *compilationUnit, const QList<const QV4::CompiledData::Binding *> &bindings) override;
    void applyBindings(QObject *obj, QV4::CompiledData::CompilationUnit *compilationUnit, const QList<const QV4::CompiledData::Binding *> &bindings) override;
#endif

private:
#if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
    void verifyProperty(const QQmlRefPointer<QV4::ExecutableCompilationUnit> &compilationUnit, const QV4::CompiledData::Binding *binding);
#elif QT_VERSION >= QT_VERSION_CHECK(5, 12, 0)
    void verifyProperty(const QQmlRefPointer<QV4::CompiledData::CompilationUnit> &compilationUnit, const QV4::CompiledData::Binding *binding);
#else
    void verifyProperty(const QV4::CompiledData::Unit *compilationUnit, const QV4::CompiledData::Binding *binding);
#endif
};

UT_NAMESPACE_END

#endif // UCSTYLEHINTS_P_H
