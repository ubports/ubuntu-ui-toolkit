/*
 * Copyright (C) 2016 Canonical, Ltd.
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
 * Authored by: Zsombor Egri <zsombor.egri@canonical.com>
 */

#ifndef MOUSETOUCHADAPTOR_P
#define MOUSETOUCHADAPTOR_P

#include "mousetouchadaptor.h"
#include <QtCore/private/qobject_p.h>
#include <QtCore/QAbstractNativeEventFilter>
#include <QWindow>
#include <xcb/xcb.h>

namespace UbuntuToolkit {

class MouseTouchAdaptorPrivate : public QObjectPrivate, public QAbstractNativeEventFilter
{
    Q_DECLARE_PUBLIC(MouseTouchAdaptor)
public:
    MouseTouchAdaptorPrivate() : QObjectPrivate() {}
    ~MouseTouchAdaptorPrivate();

    virtual void init() {}
    virtual bool nativeEventFilter(const QByteArray & eventType, void *message, long *result)
            { Q_UNUSED(eventType); Q_UNUSED(message); Q_UNUSED(result); return false; }
    bool isEnabled() const;
    virtual void setEnabled(bool enabled);

    QWindow *findQWindowWithXWindowID(WId windowId);

    // fields
    bool enabled{false};
};

#ifdef UBUNTUTOOLKIT_ENABLE_X11_TOUCH_EMULATION
class X11MouseTouchAdaptorPrivate : public MouseTouchAdaptorPrivate
{
    Q_DECLARE_PUBLIC(MouseTouchAdaptor)
public:
    X11MouseTouchAdaptorPrivate();

    void init() Q_DECL_OVERRIDE;
    bool nativeEventFilter(const QByteArray & eventType, void *message, long *result) Q_DECL_OVERRIDE;
    void setEnabled(bool value);

    bool xi2HandleEvent(xcb_ge_event_t *event);
    bool handleButtonPress(WId windowId, uint32_t detail, uint32_t modifiers, int x, int y);
    bool handleButtonRelease(WId windowId, uint32_t detail, uint32_t modifiers, int x, int y);
    bool handleMotionNotify(WId windowId, uint32_t modifiers, int x, int y);

    bool m_leftButtonIsPressed;
    bool m_triPressModifier;

    bool m_xi2Enabled{false};
    int m_xi2Minor{-1};
    int m_xiOpCode, m_xiEventBase, m_xiErrorBase;
};
#endif

} // namespace UbuntuToolkit

#endif // MOUSETOUCHADAPTOR_P

