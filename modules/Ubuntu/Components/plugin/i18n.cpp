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

#include <QtDeclarative/QDeclarativeExtensionPlugin>

UbuntuI18n::UbuntuI18n(QObject* parent) : QObject(parent)
{
    // TODO: init
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

QString UbuntuI18n::tr(QString text)
{
//    if (text == "") return "no string?";
    QString newtext = QString::fromUtf8(dgettext("ubuntu-sdk", text.toUtf8()));
    return newtext;
}
