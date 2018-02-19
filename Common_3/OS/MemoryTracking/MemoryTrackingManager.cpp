/*
 * Copyright (c) 2018 Confetti Interactive Inc.
 *
 * This file is part of The-Forge
 * (see https://github.com/ConfettiFX/The-Forge).
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
**/

// TODO: Add a memory tracker

#include "../Interfaces/ILogManager.h"
#include "../Interfaces/IOperatingSystem.h"
#include "../Interfaces/IMemoryManager.h"

#include <new>
#include <time.h>
#ifdef USE_MEMORY_TRACKING
#include "../../ThirdParty/OpenSource/FluidStudios/MemoryManager/nommgr.h"
#define	malloc(sz)	m_allocator  (__FILE__,__LINE__,__FUNCTION__,m_alloc_malloc,sz)
#define	free(ptr)	m_deallocator(__FILE__,__LINE__,__FUNCTION__,m_alloc_free,ptr)

//#define ALLOW_DLMALLOC
#undef conf_malloc
#undef conf_free

void* conf_malloc(size_t size)
{
	return malloc(size);
}

void conf_free(void* ptr)
{
	free(ptr);
}

// Just include the cpp here so we don't have to add it to the all projects
#include "../../ThirdParty/OpenSource/FluidStudios/MemoryManager/mmgr.cpp"
#else
#undef malloc
#undef calloc
#undef realloc
#undef free
#include <cstdlib>

//#define ONLY_MSPACES 1
#define MSPACES 1
#define USE_DL_PREFIX 1
#define MALLOC_ALIGNMENT ( confetti::DEFAULT_ALIGNMENT )
//#include "malloc-2.8.6.h"
#include "dlmalloc.cc"

void *conf_malloc( size_t size ) {
    return confetti::allocate( size );
}

void *conf_calloc( size_t count, size_t size ) {
    return confetti::allocate( size * count );
}

void *conf_realloc( void *p, size_t size ) {
    return confetti::reallocate( p, size );
}

void conf_free( void *p ) {
    confetti::deallocate( p );
}

static thread_local mspace tlms = create_mspace(0, 0);

namespace confetti {
    void *allocate( size_t size, size_t alignment ) {
#if defined( ALLOW_DLMALLOC )

        (void) alignment;
        return dlmalloc( size );

#elif defined( __ANDROID__ )
        void *p = nullptr;
        ::posix_memalign( &p, alignment, size );
        return p;
#else
        (void) alignment;
        return ::malloc( size );
        //return ::_aligned_malloc( size, alignment );
#endif
    }

    void *reallocate( void *p, size_t size, size_t alignment ) {
#if defined( ALLOW_DLMALLOC )

        (void) alignment;
        return dlrealloc( p, size );

#elif defined( __ANDROID__ )
        static_assert( true, "Not implemented." );
#else
        (void) alignment;
        return ::realloc( p, size );
        //return ::_aligned_realloc( p, size, alignment );
#endif
    }

    void deallocate( void *p ) {
#if defined( ALLOW_DLMALLOC )

        return dlfree( p );

#elif defined( __ANDROID__ )
        ::free( p );
#else
        ::free( p );
        //::_aligned_free( p );
#endif
    }

    void *threadLocalAllocate( size_t size, size_t alignment ) {
        (void) alignment;
        return mspace_malloc( tlms, size );
    }

    void *threadLocalReallocate( void *p, size_t size, size_t alignment ) {
        (void) alignment;
        return mspace_realloc( tlms, p, size );
    }

    void threadLocalDeallocate( void *p ) {
        mspace_free( tlms, p );
    }
}

void *operator new( size_t size ) {
    return confetti::allocate( size );
}
#endif

void *operator new[]( size_t size ) {
    return confetti::allocate( size );
}

void *operator new[]( size_t size, const char * /*name*/, int /*flags*/, unsigned /*debugFlags*/, const char * /*file*/, int /*line*/ ) {
    return confetti::allocate( size );
}

void *operator new[]( size_t size,
                      size_t alignment,
                      size_t /*alignmentOffset*/,
                      const char * /*name*/,
                      int /*flags*/,
                      unsigned /*debugFlags*/,
                      const char * /*file*/,
                      int /*line*/ ) {
    return confetti::allocate( size, alignment );
}

void *operator new( size_t size, size_t alignment ) {
    return confetti::allocate( size, alignment );
}

void *operator new( size_t size, size_t alignment, const std::nothrow_t & ) throw( ) {
    return confetti::allocate( size, alignment );
}

void *operator new[]( size_t size, size_t alignment ) {
    return confetti::allocate( size, alignment );
}

void *operator new[]( size_t size, size_t alignment, const std::nothrow_t & ) throw( ) {
    return confetti::allocate( size, alignment );
}

// C++14 deleter
void operator delete( void *p, std::size_t sz ) throw( ) {
    confetti::deallocate( p );
    (void) ( sz );
}

void operator delete[]( void *p, std::size_t sz ) throw( ) {
    confetti::deallocate( p );
    (void) ( sz );
}

void operator delete( void *p ) throw( ) {
    confetti::deallocate( p );
}

void operator delete[]( void *p ) throw( ) {
    confetti::deallocate( p );
}
