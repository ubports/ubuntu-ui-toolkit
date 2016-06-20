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
 */

#ifndef UCNAMESPACE_H
#define UCNAMESPACE_H

#include <QtCore/QObject>
#include <quickutils.h>
#include <ubuntutoolkitglobal.h>

UT_NAMESPACE_BEGIN

#define UC_QML_DEPRECATION_WARNING(msg) \
    { \
        static bool loggedOnce = false; \
        if (!loggedOnce) { \
            if (QuickUtils::showDeprecationWarnings()) { \
                qmlInfo(this) << msg; \
            } \
        } \
    }

class UCNamespace : public QObject
{
    Q_OBJECT
    Q_ENUMS(CaptionsStyle)

public:
    enum CaptionsStyle {
        TitleCaptionStyle = 5000,
        SummaryCaptionStyle
    };
    explicit UCNamespace(QObject *parent = 0);
};

// this class is for next version, no need to revision it
class UCNamespaceV13 : public UCNamespace
{
    Q_OBJECT
    Q_PROPERTY(quint16 toolkitVersion READ toolkitVersion NOTIFY toolkitVersionChanged)
    Q_PROPERTY(quint16 toolkitVersionMajor READ toolkitVersionMajor NOTIFY toolkitVersionChanged)
    Q_PROPERTY(quint16 toolkitVersionMinor READ toolkitVersionMinor NOTIFY toolkitVersionChanged)
public:
    explicit UCNamespaceV13(QObject *parent = 0) : UCNamespace(parent)
    {
        m_toolkitVersion = LATEST_UITK_VERSION;
    }

    Q_INVOKABLE quint16 version(quint8 major, quint8 minor);

Q_SIGNALS:
    void toolkitVersionChanged();

protected:
    virtual quint16 toolkitVersion() const
    {
        return m_toolkitVersion;
    }
    quint16 toolkitVersionMajor() const
    {
        return MAJOR_VERSION(m_toolkitVersion);
    }
    quint16 toolkitVersionMinor() const
    {
        return MINOR_VERSION(m_toolkitVersion);
    }

private:
    quint16 m_toolkitVersion;
};

UT_NAMESPACE_END

#endif // UCNAMESPACE_H
