// Copyright © 2016 Canonical Ltd.
// Authors: Loïc Molinari <loic.molinari@canonical.com>
//          Albert Astals Cid <albert.astals@canonical.com>
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

#ifndef LTTNG_P_H
#define LTTNG_P_H

#include <stdint.h>

typedef struct _UMLTTNGProcessEvent UMLTTNGProcessEvent;
typedef struct _UMLTTNGFrameEvent UMLTTNGFrameEvent;
typedef struct _UMLTTNGWindowEvent UMLTTNGWindowEvent;
typedef struct _UMLTTNGGenericEvent UMLTTNGGenericEvent;

struct UMLTTNGPlugin {
    void (*logProcessEvent)(UMLTTNGProcessEvent*);
    void (*logFrameEvent)(UMLTTNGFrameEvent*);
    void (*logWindowEvent)(UMLTTNGWindowEvent*);
    void (*logGenericEvent)(UMLTTNGGenericEvent*);
};

struct _UMLTTNGProcessEvent {
    uint32_t vszMemory;
    uint32_t rssMemory;
    uint16_t cpuUsage;
    uint16_t threadCount;
};

struct _UMLTTNGFrameEvent {
    uint32_t window;
    uint32_t number;
    float deltaTime;
    float syncTime;
    float renderTime;
    float gpuTime;
    float swapTime;
};

struct _UMLTTNGWindowEvent {
    const char* state;
    uint32_t id;
    uint16_t width;
    uint16_t height;
};

struct _UMLTTNGGenericEvent {
    uint32_t id;
    // Keep the size in sync with UMGenericEvent::maxStringSize. Unfortunately,
    // a static assert is not possible here since it's C and we can't include
    // the events.h C++ header.
    char string[64];
};

#endif  // LTTNG_P_H
