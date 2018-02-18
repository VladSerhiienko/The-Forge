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
*/

#pragma once

#include <new>

namespace confetti {
    /**
     * The default alignment for memory allocations.
     */
    static const size_t DEFAULT_ALIGNMENT = sizeof( void* ) << 1;

    /**
     * The regular malloc call with aligment.
     * Do not align with less then kDefaultAlignment bytes.
     * @param size The byte size of the memory chunk.
     * @param alignment The byte alignment of the memory chunk.
     * @return The allocated memory chunk address.
     */
    void* allocate( size_t size, size_t alignment = DEFAULT_ALIGNMENT );

    /**
     * The regular realloc call with aligment.
     * Do not align with less then kDefaultAlignment bytes.
     * @param size The byte size of the memory chunk.
     * @param alignment The byte alignment of the memory chunk.
     * @return The allocated memory chunk address.
     */
    void* reallocate( void* p, size_t size, size_t alignment = DEFAULT_ALIGNMENT );

    /**
     * The regular free call.
     * @param p The memory chunk address.
     */
    void deallocate( void* p );

    /**
     * The malloc call with aligment from the thread-local memory space.
     * Do not align with less then kDefaultAlignment bytes.
     * @param size The byte size of the memory chunk.
     * @param alignment The byte alignment of the memory chunk.
     * @return The allocated memory chunk address.
     */
    void* threadLocalAllocate( size_t size, size_t alignment = DEFAULT_ALIGNMENT );

    /**
     * The realloc call with aligment from the thread-local memory space.
     * Do not align with less then kDefaultAlignment bytes.
     * @param p Address of the old memroy chunk.
     * @param size The byte size of the memory chunk.
     * @param alignment The byte alignment of the memory chunk.
     * @return The reallocated memory chunk address.
     */
    void* threadLocalReallocate( void* p, size_t size, size_t alignment = DEFAULT_ALIGNMENT );

    /**
     * The free call from the thread-local memory space.
     * @param p The memory chunk address.
     */
    void threadLocalDeallocate( void* p );

    template < size_t uAlignment = DEFAULT_ALIGNMENT, bool bThreadLocal = false >
    struct TNew {
        inline static void* operator new( size_t size ) {
            if ( bThreadLocal )
                return apemode::thread_local_malloc( size, uAlignment );
            else
                return apemode::malloc( size, uAlignment );
        }

        inline static void* operator new[]( size_t size ) {
            if ( bThreadLocal )
                return apemode::thread_local_malloc( size, uAlignment );
            else
                return apemode::malloc( size, uAlignment );
        }

        inline static void operator delete( void* ptr ) {
            if ( bThreadLocal )
                apemode::thread_local_free( ptr );
            else
                apemode::free( ptr );
        }

        inline static void operator delete[]( void* ptr ) {
            if ( bThreadLocal )
                apemode::thread_local_free( ptr );
            else
                apemode::free( ptr );
        }
    };

    template < typename TDerived, bool bThreadLocal = false >
    struct TNewClass : TNew< alignof( TDerived ), bThreadLocal > {};

} // namespace confetti

//
// Old interface
//

void* conf_malloc( size_t size );
void* conf_calloc( size_t count, size_t size );
void* conf_realloc( void* p, size_t size );
void  conf_free( void* p );

#pragma push_macro("new")
#undef new

template < typename T, typename... Args >
static T* conf_placement_new( void* ptr, Args... args ) {
    return new ( ptr ) T( args... );
}

#pragma pop_macro( "new" )
