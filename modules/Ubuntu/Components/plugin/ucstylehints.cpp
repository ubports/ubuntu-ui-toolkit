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
#include <QtQml/QQmlInfo>

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

    if (binding->type == QV4::CompiledData::Binding::Type_GroupProperty
            || binding->type == QV4::CompiledData::Binding::Type_AttachedProperty) {
        const QV4::CompiledData::Object *subObj = qmlUnit->objectAt(binding->value.objectIndex);
        const QV4::CompiledData::Binding *subBinding = subObj->bindingTable();
        for (quint32 i = 0; i < subObj->nBindings; ++i, ++subBinding) {
            verifyProperty(qmlUnit, subBinding);
        }
    }
}

void UCStyleHintsParser::applyBindings(QObject *obj, QQmlCompiledData *cdata, const QList<const QV4::CompiledData::Binding *> &bindings)
{
    UCStyleHints *hints = static_cast<UCStyleHints*>(obj);

    hints->m_bindings = bindings;
    hints->m_cdata = cdata;
    hints->m_decoded = true;
}

/*!
 *
 */
UCStyleHints::UCStyleHints(QObject *parent)
    : QObject(parent)
    , m_decoded(false)
    , m_completed(false)
{
}

void UCStyleHints::classBegin()
{

}

void UCStyleHints::componentComplete()
{
    if (!m_styledItem) {
        qmlInfo(this) << "StyleHints must be declared as property value for StyledItem or a derivate of it.";
    }
    if (parent()->findChildren<UCStyleHints*>().size() > 1) {
        qmlInfo(this) << "There can be only one StyleHints in a StyledItem.";
        return;
    }
    m_completed = true;
    _q_applyStyleHints();
}

void UCStyleHints::setStyledItem(UCStyledItemBase *item)
{
    m_styledItem = item;
    if (m_styledItem) {
        connect(m_styledItem, SIGNAL(themeChanged()),
                this, SLOT(_q_applyStyleHints()));
        connect(m_styledItem, SIGNAL(styleNameChanged()),
                this, SLOT(_q_applyStyleHints()));
        setParent(item);
    }
}

void UCStyleHints::unsetStyledItem()
{
    if (m_styledItem) {
        disconnect(m_styledItem, SIGNAL(themeChanged()),
                   this, SLOT(_q_applyStyleHints()));
        disconnect(m_styledItem, SIGNAL(styleNameChanged()),
                   this, SLOT(_q_applyStyleHints()));
    }
    m_styledItem.clear();
}

void UCStyleHints::_q_applyStyleHints()
{
    if (!m_completed) {
        return;
    }
    Q_FOREACH(const QV4::CompiledData::Binding *binding, m_bindings) {
        applyProperty(NULL, QString(), m_cdata->compilationUnit->data, binding);
    }
}

void UCStyleHints::applyProperty(QObject *valueSet, const QString &propertyPrefix, const QV4::CompiledData::Unit *qmlUnit, const QV4::CompiledData::Binding *binding)
{

}

