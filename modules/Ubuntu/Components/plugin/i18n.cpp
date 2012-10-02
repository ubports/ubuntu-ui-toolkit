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
 * Author: Juhapekka Piiroinen <juhapekka.piiroinen@canonical.com>
 */

#include "i18n.h"

// libc
#include <libintl.h>

UbuntuI18n::UbuntuI18n(QObject* parent) : QObject(parent)
{
    // nothing to do. Initialization is done in init().
//    this->domain = "ubuntu-sdk";
    this->m_domain = QString("ubuntu-sdk");
}

UbuntuI18n::~UbuntuI18n()
{
    // nothing to do
}

void UbuntuI18n::init(const char* domain, const char* localeDir)
{
    setlocale(LC_ALL, "");
    bindtextdomain(domain, localeDir);
    textdomain(domain);
}

//QString UbuntuI18n::domain() const {
//    Q_D(const UbuntuI18n);
//    return d->domain;
//}

//QString UbuntuI18n::localeDir() {
//    return this->localeDir;
//}

//void UbuntuI18n::setDomain(QString d) {
//    this->domain = d;
//}

QString UbuntuI18n::domain() {
    return this->m_domain;
}

//void UbuntuI18n::setLocaleDir(QString ld) {
//    this->localeDir = ld;
//}

//void UbuntuI18n::domainChanged() {
//    this->init(this->domain, this->localeDir);
//}

//void UbuntuI18n::localeDirChanged() {
//    this->init(this->domain, this->localeDir);
//}

QString UbuntuI18n::tr(const QString& text, const QString& domain)
{
    if (domain.isNull()) {
        return QString::fromUtf8(dgettext(NULL, text.toUtf8().constData()));
    } else {
        return QString::fromUtf8(dgettext(domain.toUtf8().constData(), text.toUtf8().constData()));
    }
}

QString UbuntuI18n::tr(const QString& singular, const QString& plural, int n, const QString& domain) {
    if (domain.isNull()) {
        return QString::fromUtf8(dngettext(NULL, singular.toUtf8().constData(), plural.toUtf8().constData(), n));
    } else {
        return QString::fromUtf8(dngettext(domain.toUtf8().constData(), singular.toUtf8().constData(), plural.toUtf8().constData(), n));
    }
}
