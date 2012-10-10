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

#ifndef UBUNTU_COMPONENTS_I18N_H
#define UBUNTU_COMPONENTS_I18N_H

#include <QObject>
//#include <QtDeclarative>
//#include <QDeclarativeListProperty>

class UbuntuI18n : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString domain READ domain WRITE setDomain)
    Q_PROPERTY(QString localeDir READ localeDir WRITE setLocaleDir)



public:
    UbuntuI18n(QObject* parent = NULL);
    ~UbuntuI18n();

    /**
     * @brief Install the gettext catalog.
     * @param domain The domain of the catalog.
     * @param localeDir The path to the catalog.
     */
//    void init(const char* domain, const char* localeDir);
    void init(QString domain, QString localeDir);

//    /**
//     * Add an object named "i18n" to context.
//     *
//     * One can then get translations with i18n.tr("english text")
//     */
//    void qmlInit(QDeclarativeContext* context);

    /**
     * @brief Translate the given string using gettext.
     * @param text The text to translate.
     * @param domain The domain to use for the translation.
     */
    Q_INVOKABLE QString tr(const QString& text, const QString& domain = QString());

    /**
     * @brief Translate the given string using gettext. Should be called like this:
     *          tr("%n file", "%n files", count, domain)
     * @param singular The singular version of the text to translate.
     * @param plural The plural version of the text to translate.
     * @param n Number of items.
     * @param domain The domain to use for the translation.
     */
    Q_INVOKABLE QString tr(const QString& singular, const QString& plural, int n, const QString& domain = QString());

    // TODO: Add functions where a context is supplied.

    QString domain();
    QString localeDir();
    void setDomain(QString domain);
    void setLocaleDir(QString localeDir);


//Q_SIGNALS:

//    void domainChanged();
//    void localeDirChanged();

private:

    QString _domain;
    QString _localeDir;
    QString _emptyString;
};

#endif // UBUNTU_COMPONENTS_I18N_H
