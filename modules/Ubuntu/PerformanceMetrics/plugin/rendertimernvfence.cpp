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

#include "rendertimernvfence.h"

RenderTimerNVFence::RenderTimerNVFence()
{
}

bool RenderTimerNVFence::isAvailable()
{
    QList<QByteArray> glExtensions = QByteArray(
        reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS))).split(' ');
    return glExtensions.contains("GL_NV_fence");
}

void RenderTimerNVFence::setup()
{
    m_fenceNV.genFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, GLuint*)>(
        eglGetProcAddress("glGenFencesNV"));
    m_fenceNV.deleteFencesNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLsizei, const GLuint*)>(
        eglGetProcAddress("glDeleteFencesNV"));
    m_fenceNV.setFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint, GLenum)>(
        eglGetProcAddress("glSetFenceNV"));
    m_fenceNV.finishFenceNV = reinterpret_cast<void (QOPENGLF_APIENTRYP)(GLuint)>(
        eglGetProcAddress("glFinishFenceNV"));
    fenceSystem_ = FenceNV;
    m_fenceNV.genFencesNV(2, m_fence);
}

void RenderTimerNVFence::teardown()
{
    m_fenceNV.deleteFencesNV(2, m_fence);

}

void RenderTimerNVFence::start()
{
    m_fenceNV.setFenceNV(m_fence[0], GL_ALL_COMPLETED_NV);
}

qint64 RenderTimerNVFence::stop()
{
    QElapsedTimer timer;
    m_fenceNV.setFenceNV(m_fence[1], GL_ALL_COMPLETED_NV);
    m_fenceNV.finishFenceNV(m_fence[0]);
    qint64 beforeTime = timer.nsecsElapsed();
    m_fenceNV.finishFenceNV(m_fence[1]);
    qint64 afterTime = timer.nsecsElapsed();
    return afterTime - beforeTime;
}
