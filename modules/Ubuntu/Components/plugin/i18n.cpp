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

#include "i18n.h"

// libc
#include <libintl.h>
//#include "/usr/share/gettext/gettext.h"
//#include <gettext-po.h>
#include <QDir>
#include <QtCore>
#include <QtQml/QQmlContext>

//#include <stdio.h>

UbuntuI18n::UbuntuI18n(QObject* parent) : QObject(parent)
{
}

QString UbuntuI18n::domain() {
    return _domain;
}

void UbuntuI18n::setDomain(QString domain) {
    _domain = domain;
    textdomain(domain.toUtf8().constData());
    Q_EMIT domainChanged();
}

QString UbuntuI18n::tr(const QString& text)
{
    return QString::fromUtf8(gettext(text.toUtf8()));
}

QString UbuntuI18n::ntr(const QString &singular, const QString &plural, int n)
{
    return QString::fromUtf8(ngettext(singular.toUtf8(), plural.toUtf8(), n));
}

QString UbuntuI18n::dtr(const QString& domain, const QString& text)
{
    if (domain.isNull()) {
        return QString::fromUtf8(dgettext(NULL, text.toUtf8().constData()));
    } else {
        return QString::fromUtf8(dgettext(domain.toUtf8().constData(), text.toUtf8().constData()));
    }
}

QString UbuntuI18n::dntr(const QString& domain, const QString& singular, const QString& plural, int n)
{
    if (domain.isNull()) {
        return QString::fromUtf8(dngettext(NULL, singular.toUtf8().constData(), plural.toUtf8().constData(), n));
    } else {
        return QString::fromUtf8(dngettext(domain.toUtf8().constData(), singular.toUtf8().constData(), plural.toUtf8().constData(), n));
    }
}

// TODO: The code below is the same as in resolution_independence branch
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
