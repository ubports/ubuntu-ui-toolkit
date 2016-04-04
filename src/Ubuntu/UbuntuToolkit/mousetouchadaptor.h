/*
 * Copyright (C) 2013,2015 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Daniel d'Andrada <daniel.dandrada@canonical.com>
 */

#ifndef MOUSE_TOUCH_ADAPTOR_H
#define MOUSE_TOUCH_ADAPTOR_H

#include "ubuntutoolkitglobal.h"

class QMouseEvent;
class QTouchDevice;

namespace UbuntuToolkit {

// Transforms QMouseEvents into single-finger QTouchEvents.
class MouseTouchAdaptorPrivate;
class UBUNTUTOOLKIT_EXPORT MouseTouchAdaptor : public QObject
{
    Q_OBJECT
public:
    explicit MouseTouchAdaptor();

    static bool registerTouchDevice();
    inline static QTouchDevice *touchDevice()
    {
        return m_touchDevice;
    }

    Q_PRIVATE_PROPERTY(MouseTouchAdaptor::d_func(), bool enabled READ isEnabled WRITE setEnabled NOTIFY enabledChanged)

    bool enabled() const;
    void setEnabled(bool value);

Q_SIGNALS:
    void enabledChanged(bool value);

private:
    Q_DECLARE_PRIVATE(MouseTouchAdaptor)
    static QTouchDevice *m_touchDevice;
};

} // namespace UbuntuToolkit

#endif // MOUSE_TOUCH_ADAPTOR_H
