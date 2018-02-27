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
#include "../../ThirdParty/OpenSource/TinySTL/vector.h"
#include "../../ThirdParty/OpenSource/TinySTL/string.h"
#include "../../OS/Interfaces/IThread.h"

#include "../../ThirdParty/OpenSource/spdlog/include/spdlog/spdlog.h"

enum LogLevel {
    LL_Debug   = spdlog::level::debug,
    LL_Info    = spdlog::level::info,
    LL_Warning = spdlog::level::warn,
    LL_Error   = spdlog::level::err,
    LL_Raw     = spdlog::level::info,
    LL_None    = spdlog::level::trace,
};

class File;

/// Logging subsystem.
class LogManager {
public:
    LogManager( );
    LogManager( LogLevel level );
    ~LogManager( );

    void Open( const String& fileName );
    void Close( );

    void     SetLevel( LogLevel level );
    LogLevel GetLevel( ) const;

    static void Write( int level, const String& message );
    static void WriteRaw( const String& message, bool error = false );

    inline static spdlog::level::level_enum ToSpdLogLevel( const int logLevel ) {
        return static_cast< spdlog::level::level_enum >( logLevel );
    }

    template < typename... Args >
    inline static void WriteSpd( int level, const char* fmt, const Args&... args ) {
        pLogInstance->mSpdLogger->log( ToSpdLogLevel( level ), fmt, args... );
    }

    template < typename... Args >
    inline static void Info( const char* fmt, const Args&... args ) {
        pLogInstance->mSpdLogger->log( spdlog::level::info, fmt, args... );
    }

    template < typename... Args >
    inline static void Warn( const char* fmt, const Args&... args ) {
        pLogInstance->mSpdLogger->log( spdlog::level::warn, fmt, args... );
    }

    template < typename... Args >
    inline static void Error( const char* fmt, const Args&... args ) {
        pLogInstance->mSpdLogger->log( spdlog::level::err, fmt, args... );
    }

private:
    std::shared_ptr< spdlog::logger > mSpdLogger;
    LogLevel                          mLogLevel;
    static LogManager*                pLogInstance;
};

String ToString( const char* formatString, const char* function, ... );
