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

#include "GettextBinding.h"

namespace C {
#include <libintl.h>
}

#include <QtQml>

GettextBinding::GettextBinding(QObject* parent) : QObject(parent)
{
}

void GettextBinding::qmlRegisterTypes(const char* uri)
{
    qmlRegisterUncreatableType<GettextBinding>(uri, 0, 1, "gettext", "Singleton object");
}

void GettextBinding::qmlInit(QQmlContext *context)
{
    context->setContextProperty("gettext", &GettextBinding::instance());
    static ContextPropertyChangeListener i18nChangeListener(context, "gettext");
    QObject::connect(&GettextBinding::instance(), SIGNAL(domainChanged()),
                 &i18nChangeListener, SLOT(updateContextProperty()));
}

QString GettextBinding::domain() {
    return _domain;
}

void GettextBinding::bindtextdomain(const QString& domain_name, const QString& dir_name) {
    C::bindtextdomain(domain_name.toUtf8(), dir_name.toUtf8());
}

void GettextBinding::setDomain(QString domain) {
    _domain = domain;
    C::textdomain(domain.toUtf8().constData());
    Q_EMIT domainChanged();
}

QString GettextBinding::gettext(const QString& text)
{
    return QString::fromUtf8(C::gettext(text.toUtf8()));
}

QString GettextBinding::ngettext(const QString &singular, const QString &plural, int n)
{
    return QString::fromUtf8(C::ngettext(singular.toUtf8(), plural.toUtf8(), n));
}

QString GettextBinding::dgettext(const QString& domain, const QString& text)
{
    if (domain.isNull()) {
        return QString::fromUtf8(C::dgettext(NULL, text.toUtf8()));
    } else {
        return QString::fromUtf8(C::dgettext(domain.toUtf8(), text.toUtf8()));
    }
}

QString GettextBinding::dngettext(const QString& domain, const QString& singular, const QString& plural, int n)
{
    if (domain.isNull()) {
        return QString::fromUtf8(C::dngettext(NULL, singular.toUtf8(), plural.toUtf8(), n));
    } else {
        return QString::fromUtf8(C::dngettext(domain.toUtf8(), singular.toUtf8(), plural.toUtf8(), n));
    }
}

// TODO: The code below is the same as in units.cpp (resolution_independence branch)
//  create a separate cpp file with this code?
ContextPropertyChangeListener::ContextPropertyChangeListener(QQmlContext *context,QString contextProperty) :
    m_context(context),
    m_contextProperty(contextProperty)
{
}

void ContextPropertyChangeListener::updateContextProperty()
{
    QVariant value = m_context->contextProperty(m_contextProperty);
    m_context->setContextProperty(m_contextProperty, value);
}
