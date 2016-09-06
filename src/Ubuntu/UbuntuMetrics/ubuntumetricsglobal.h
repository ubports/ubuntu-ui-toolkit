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

#ifndef UBUNTUMETRICSGLOBAL_H
#define UBUNTUMETRICSGLOBAL_H

#include <QtCore/qglobal.h>

#if defined(QT_BUILD_UBUNTUMETRICS_LIB)
#define UBUNTU_METRICS_EXPORT Q_DECL_EXPORT
#else
#define UBUNTU_METRICS_EXPORT Q_DECL_IMPORT
#endif

#endif  // UBUNTUMETRICSGLOBAL_H
