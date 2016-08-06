// Copyright © 2016 Canonical Ltd.
// Author: Loïc Molinari <loic.molinari@canonical.com>
//
// This file is part of Ubuntu UI Toolkit.
//
// Ubuntu UI Toolkit is free software: you can redistribute it and/or modify it
// under the terms of the GNU Lesser General Public License as published by the
// Free Software Foundation; version 3.
//
// Ubuntu UI Toolkit is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License
// for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with Ubuntu UI Toolkit. If not, see <http://www.gnu.org/licenses/>.

#ifndef LOGGER_P_H
#define LOGGER_P_H

#include "logger.h"
#include "ubuntumetricsglobal_p.h"
#include "events.h"
#include <QtCore/QFile>
#include <QtCore/QTextStream>

class UBUNTU_METRICS_PRIVATE_EXPORT UMFileLoggerPrivate
{
public:
    enum {
        Open     = (1 << 0),
        Colored  = (1 << 1),
        Parsable = (1 << 2)
    };

    UMFileLoggerPrivate(const QString& fileName, bool parsable);
    UMFileLoggerPrivate(FILE* fileHandle, bool parsable);

    void log(const UMEvent& event);

    QFile m_file;
    QTextStream m_textStream;
    quint8 m_flags;
};

#endif  // LOGGER_P_H
