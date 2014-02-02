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

#include "rendertimerextquery.h"
#include <QtGui/QOpenGLContext>

#if !defined(GL_TIME_ELAPSED)
#define GL_TIME_ELAPSED 0x88BF
#endif

RenderTimerEXTQuery::RenderTimerEXTQuery()
{
}

bool RenderTimerEXTQuery::isAvailable()
{
    QOpenGLContext* context = QOpenGLContext::currentContext();
    return context->hasExtension(QByteArrayLiteral("GL_EXT_timer_query"));
}

void RenderTimerEXTQuery::setup()
{
    QOpenGLContext* context = QOpenGLContext::currentContext();
    timerQuery_.genQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
        context->getProcAddress("glGenQueries"));
    timerQuery_.deleteQueries = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
        context->getProcAddress("glDeleteQueries"));
    timerQuery_.beginQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum, GLuint)>(
        context->getProcAddress("glBeginQuery"));
    timerQuery_.endQuery = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLenum)>(
        context->getProcAddress("glEndQuery"));
    timerQuery_.getQueryObjectui64vExt =
    reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum, GLuint64EXT*)>(
        context->getProcAddress("glGetQueryObjectui64vEXT"));
    timerQueryVersion_ = TimerQueryExt;
    timerQuery_.genQueries(1, timer_);
}

void RenderTimerEXTQuery::teardown()
{
    timerQuery_.deleteQueries(1, timer_);
}

void RenderTimerEXTQuery::start()
{
    timerQuery_.beginQuery(GL_TIME_ELAPSED, timer_[0]);
}

qint64 RenderTimerEXTQuery::stop()
{
    GLuint64EXT time;
    timerQuery_.endQuery(GL_TIME_ELAPSED);
    timerQuery_.getQueryObjectui64vExt(timer_[0], GL_QUERY_RESULT, &time);
    return static_cast<qint64>(time);
}
