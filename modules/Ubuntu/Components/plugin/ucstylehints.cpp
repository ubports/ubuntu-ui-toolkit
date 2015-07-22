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

#include "ucstylehints.h"
#include "ucstyleditembase_p.h"
#include "propertychange_p.h"
#include <QtQml/QQmlInfo>

// verifies property declaration correctness
void UCStyleHintsParser::verifyBindings(const QV4::CompiledData::Unit *qmlUnit, const QList<const QV4::CompiledData::Binding *> &bindings)
{
    Q_FOREACH(const QV4::CompiledData::Binding *binding, bindings) {
        verifyProperty(qmlUnit, binding);
    }
}

void UCStyleHintsParser::verifyProperty(const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding)
{
    if (binding->type == QV4::CompiledData::Binding::Type_Object) {
        error(qmlUnit->objectAt(binding->value.objectIndex), "StyleHints does not support creating state-specific objects.");
        return;
    }

    // group properties or attached properties, we do handle those as well
    if (binding->type == QV4::CompiledData::Binding::Type_GroupProperty
            || binding->type == QV4::CompiledData::Binding::Type_AttachedProperty) {
        const QV4::CompiledData::Object *subObj = qmlUnit->objectAt(binding->value.objectIndex);
        const QV4::CompiledData::Binding *subBinding = subObj->bindingTable();
        for (quint32 i = 0; i < subObj->nBindings; ++i, ++subBinding) {
            verifyProperty(qmlUnit, subBinding);
        }
    }

    // filter out signals!
    QString propertyName = qmlUnit->stringAt(binding->propertyNameIndex);
    if (propertyName.startsWith("on") && propertyName.at(2).isUpper()) {
        error(binding, "Signal properties are not supported.");
        return;
    }
}

// decodes property declarations, stores the bindings and values
void UCStyleHintsParser::applyBindings(QObject *obj, QQmlCompiledData *cdata, const QList<const QV4::CompiledData::Binding *> &bindings)
{
    UCStyleHints *hints = static_cast<UCStyleHints*>(obj);
    QV4::CompiledData::Unit *qmlUnit = cdata->compilationUnit->data;

    UCStyledItemBase *styledItem = qobject_cast<UCStyledItemBase*>(hints->parent());
    if (!styledItem) {
        // error will be reported in componentCompleted
        return;
    }

    Q_FOREACH(const QV4::CompiledData::Binding *binding, bindings) {
        hints->decodeBinding(QString(), qmlUnit, binding);
    }

    hints->m_cdata = cdata;
    hints->m_decoded = true;
}

void UCStyleHints::decodeBinding(const QString &propertyPrefix, const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding)
{
    QString propertyName = propertyPrefix + qmlUnit->stringAt(binding->propertyNameIndex);

    // handle grouped properties first
    if (binding->type == QV4::CompiledData::Binding::Type_GroupProperty
        || binding->type == QV4::CompiledData::Binding::Type_AttachedProperty) {

        const QV4::CompiledData::Object *subObj = qmlUnit->objectAt(binding->value.objectIndex);
        const QV4::CompiledData::Binding *subBinding = subObj->bindingTable();
        QString pre = propertyName + ".";
        for (quint32 i = 0; i < subObj->nBindings; ++i, ++subBinding) {
            decodeBinding(pre, qmlUnit, subBinding);
        }
        return;
    }

    switch (binding->type) {
    case QV4::CompiledData::Binding::Type_Script:
    {
        QString expression = binding->valueAsScriptString(qmlUnit);
        QUrl url = QUrl();
        int line = -1;
        int column = -1;

        QQmlData *ddata = QQmlData::get(this);
        if (ddata && ddata->outerContext) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
            QUrl outerContextUrl(ddata->outerContext->url());
#else
            QUrl outerContextUrl(ddata->outerContext->url);
#endif
            if (!outerContextUrl.isEmpty()) {
                url = outerContextUrl;
                line = ddata->lineNumber;
                column = ddata->columnNumber;
            }
        }
        m_expressions << Expression(propertyName, binding->value.compiledScriptIndex, expression, url, line, column);
        break;
    }
    case QV4::CompiledData::Binding::Type_Translation:
    case QV4::CompiledData::Binding::Type_TranslationById:
    case QV4::CompiledData::Binding::Type_String:
    {
        m_values << qMakePair(propertyName, binding->valueAsString(qmlUnit));
        break;
    }
    case QV4::CompiledData::Binding::Type_Number:
    {
        m_values << qMakePair(propertyName, binding->valueAsNumber());
        break;
    }
    case QV4::CompiledData::Binding::Type_Boolean:
    {
        m_values << qMakePair(propertyName, binding->valueAsBoolean());
        break;
    }
    }
}

void UCStyleHints::propertyNotFound(const QString &styleName, const QString &property)
{
    if (!m_ignoreUnknownProperties) {
        qmlInfo(this) << QString("Style '%1' has no property called '%2'.").arg(styleName).arg(property);
    }
}

/*!
 * \qmltype StyleHints
 * \instantiates UCStyleHints
 * \inmodule Ubuntu Components 1.3
 * \ingroup ubuntu
 * \since Ubuntu.Components 1.3
 * \brief Component holding style specific properties to configure a particular
 * StyledItem's style runtime.
 *
 * StyleHints is a custom parser, meaning style properties can be listed without
 * any property declaration, same way as in PropertyChanges or Connections, which
 * are similar custom parsers. Properties enumerated do not have to be present in
 * the component's style as default, behavior which can be chenged by setting \l
 * ignoreUnknownProperties to true. In this case properties not found in the style
 * will be displayed as warnings.
 *
 * In the following example the Button will be drawn as white when not pressed, and
 * colored as blue when pressed.
 * \qml
 * Button {
 *     id: button
 *     StyleHints {
 *         defaultColor: button.pressed ? "blue" : "white"
 *     }
 * }
 * \endqml
 *
 * StyleHints does not support signal properties (i.e signal handlers) and object
 * declarations as property values.
 *
 * StyleHints can be only declared in a \l StyledItem or as a derivate of it.
 */

/*!
 * \qmlproperty bool StyleHints::ignoreUnknownProperties
 * The property drives whether component should warn on properties not found in
 * the component's style. The default setting is not to warn.
 */
UCStyleHints::UCStyleHints(QObject *parent)
    : QObject(parent)
    , m_decoded(false)
    , m_completed(false)
    , m_ignoreUnknownProperties(true)
{
}

UCStyleHints::~UCStyleHints()
{
    qDeleteAll(m_propertyBackup);
    m_propertyBackup.clear();
}

void UCStyleHints::classBegin()
{
    m_styledItem = qobject_cast<UCStyledItemBase*>(parent());
    if (m_styledItem) {
        connect(m_styledItem, SIGNAL(styleInstanceChanged()),
                this, SLOT(_q_applyStyleHints()));
    } else {
        qmlInfo(this) << "StyleHints must be declared in a StyledItem or a derivate of it.";
    }
}

void UCStyleHints::componentComplete()
{
    m_completed = (m_styledItem != Q_NULLPTR);
    _q_applyStyleHints();
}

// apply the style hints and check each property existence
void UCStyleHints::_q_applyStyleHints()
{
    if (!m_completed || !m_decoded || !m_styledItem || !UCStyledItemBasePrivate::get(m_styledItem)->styleItem) {
        return;
    }

    // restore properties first
    qDeleteAll(m_propertyBackup);
    m_propertyBackup.clear();

    QQuickItem *item = UCStyledItemBasePrivate::get(m_styledItem)->styleItem;
    const QMetaObject *mo = item->metaObject();
    const QString styleName = UCStyledItemBasePrivate::get(m_styledItem)->styleName();
    // apply values first
    for (int i = 0; i < m_values.size(); i++) {
        if (mo->indexOfProperty(m_values[i].first.toUtf8()) < 0) {
            propertyNotFound(styleName, m_values[i].first);
            continue;
        }
        PropertyChange *change = new PropertyChange(item, m_values[i].first.toUtf8());
        PropertyChange::setValue(change, m_values[i].second);
        m_propertyBackup << change;
    }

    QQmlContext *context = qmlContext(this);
    // then apply expressions/bindings
    for (int ii = 0; ii < m_expressions.count(); ii++) {
        Expression e = m_expressions[ii];
        PropertyChange *change = new PropertyChange(item, e.name.toUtf8());
        if (!change->property().isValid()) {
            propertyNotFound(styleName, e.name);
            delete change;
            continue;
        }

        // create a binding object from the expression using the palette context
        QQmlContextData *cdata = QQmlContextData::get(context);
        QQmlBinding *newBinding = 0;
        if (e.id != QQmlBinding::Invalid) {
            QV4::Scope scope(QQmlEnginePrivate::getV4Engine(qmlEngine(this)));
            QV4::ScopedValue function(scope, QV4::QmlBindingWrapper::createQmlCallableForFunction(cdata, item, m_cdata->compilationUnit->runtimeFunctions[e.id]));
            newBinding = new QQmlBinding(function, item, cdata);
        }
        if (!newBinding) {
            newBinding = new QQmlBinding(e.expression, item, cdata, e.url.toString(), e.line, e.column);
        }

        newBinding->setTarget(change->property());
        PropertyChange::setBinding(change, newBinding);
    }
}

