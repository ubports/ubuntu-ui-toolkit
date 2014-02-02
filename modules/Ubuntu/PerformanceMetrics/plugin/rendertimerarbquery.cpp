/*
 * Copyright 2014 Canonical Ltd.
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
 * Author: Florian Boucault <florian.boucault@canonical.com>
 */

#include "rendertimerarbquery.h"
#include <QtCore/QPair>
#include <QtGui/QOpenGLContext>

#if !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF
#endif

RenderTimerARBQuery::RenderTimerARBQuery()
{
}

bool RenderTimerARBQuery::isAvailable()
{
    // We could use the thin QOpenGLTimerQuery wrapper from Qt 5.1, but the lack of a method to check
    // the presence of glQueryCounter() would force us to inspect OpenGL version and extensions, which
    // is basically as annoying as doing the whole thing here.
    // TODO(loicm) Add an hasQuerycounter() method to QOpenGLTimerQuery.
    QOpenGLContext* context = QOpenGLContext::currentContext();
    QSurfaceFormat format = context->format();
    return qMakePair(format.majorVersion(), format.minorVersion()) >= qMakePair(3, 2)
            && context->hasExtension(QByteArrayLiteral("GL_ARB_timer_query"));
}

void RenderTimerARBQuery::setup()
{
    QOpenGLContext* context = QOpenGLContext::currentContext();
    m_timerQuery.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
        context->getProcAddress("glGenQueries"));
    m_timerQuery.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
        context->getProcAddress("glDeleteQueries"));
    m_timerQuery.getQueryObjectui64v =
    reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64*)>(
        context->getProcAddress("glGetQueryObjectui64v"));
    m_timerQuery.queryCounter = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
        context->getProcAddress("glQueryCounter"));
    m_timerQueryVersion = TimerQueryCore;
    m_timerQuery.genQueries(2, m_timer);
}

void RenderTimerARBQuery::teardown()
{
    m_timerQuery.deleteQueries(2, m_timer);
}

void RenderTimerARBQuery::start()
{
    m_timerQuery.queryCounter(m_timer[0], GL_TIMESTAMP);
}

qint64 RenderTimerARBQuery::stop()
{
    GLuint64 time[2] = { 0, 0 };
    m_timerQuery.queryCounter(m_timer[1], GL_TIMESTAMP);
    m_timerQuery.getQueryObjectui64v(m_timer[0], GL_QUERY_RESULT, &time[0]);
    m_timerQuery.getQueryObjectui64v(m_timer[1], GL_QUERY_RESULT, &time[1]);
    if (time[0] != 0 && time[1] != 0) {
        return time[1] - time[0];
    } else {
        return -1;
    }
}
