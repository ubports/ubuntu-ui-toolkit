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

#include "ubuntumetricsglobal_p.h"

void* alignedAlloc(size_t alignment, size_t size)
{
    DASSERT(IS_POWER_OF_TWO(alignment));
    // DASSERT(((size % alignment) == 0));  // FIXME(loicm) ASSERT doesn't support '%'...

#if defined(__APPLE__)
    void* pointer;
    posix_memalign(&pointer, alignment, size);
    return pointer;
#elif defined(_WIN32)
    return _aligned_malloc(size, alignment);
#else
    return aligned_alloc(alignment, size);
#endif
}
