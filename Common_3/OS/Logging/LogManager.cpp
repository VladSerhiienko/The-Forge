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

#include "../Interfaces/ILogManager.h"
#include "../Interfaces/IFileSystem.h"
#include "../Interfaces/IOperatingSystem.h"
#include "../Interfaces/IMemoryManager.h"

#include "LogManager.h"

#include "../../ThirdParty/OpenSource/spdlog/include/spdlog/formatter.h"
#include "../../ThirdParty/OpenSource/spdlog/include/spdlog/fmt/ostr.h"
#include "../../ThirdParty/OpenSource/spdlog/include/spdlog/sinks/msvc_sink.h"
#include "../../ThirdParty/OpenSource/spdlog/include/spdlog/sinks/stdout_sinks.h"

#ifdef _DEBUG
#define THEFORGE_DEFAULT_LOG_LEVEL LogLevel::LL_Debug
#else
#define THEFORGE_DEFAULT_LOG_LEVEL LogLevel::LL_Info
#endif

std::shared_ptr< spdlog::logger > CreateLogger( spdlog::level::level_enum lvl, String logFile ) {

    std::vector< spdlog::sink_ptr > sinks {
#if _WIN32
        std::make_shared< spdlog::sinks::wincolor_stdout_sink_mt >( ),
        std::make_shared< spdlog::sinks::msvc_sink_mt >( ),
        std::make_shared< spdlog::sinks::simple_file_sink_mt >( logFile.c_str( ) )
#else
        std::make_shared< spdlog::sinks::stdout_sink_mt >( ),
        std::make_shared< spdlog::sinks::simple_file_sink_mt >( logFile )
#endif
    };

    auto logger = spdlog::create<>( "the-forge", sinks.begin( ), sinks.end( ) );
    logger->set_level( lvl );

    spdlog::set_pattern( "%v" );

    logger->info( "" );
    logger->info( "  ________               ______" );
    logger->info( " /_  __/ /_  ___        / ____/___  _________ ____" );
    logger->info( "  / / / __ \\/ _ \\______/ /_  / __ \\/ ___/ __ `/ _ \\" );
    logger->info( " / / / / / /  __/_____/ __/ / /_/ / /  / /_/ /  __/" );
    logger->info( "/_/ /_/ /_/\\___/     /_/    \\____/_/   \\__, /\\___/" );
    logger->info( "                                      /____/" );
    logger->info( "" );

    spdlog::set_pattern( "%c" );
    logger->info( "" );

    spdlog::set_pattern( "[%T.%f] [%L] %v" );
    return logger;
}

LogManager* LogManager::pLogInstance = nullptr;

LogManager::LogManager( ) : LogManager( THEFORGE_DEFAULT_LOG_LEVEL ) {
}

LogManager::LogManager( LogLevel level ) : mLogLevel( level ) {
    ASSERT( nullptr == pLogInstance );

    pLogInstance = this;

    Open( FileSystem::GetCurrentDir( ) + "Log.txt" );
    Thread::SetMainThread( );
}

LogManager::~LogManager( ) {
    Close( );
    pLogInstance = nullptr;
}

void LogManager::Open( const String& fileName ) {

    if ( 0 == fileName.size( ) )
        return;

    mSpdLogger = CreateLogger( ToSpdLogLevel( mLogLevel ), FileSystem::GetCurrentDir( ) + "Log.log" );
}

void LogManager::Close( ) {
    mSpdLogger = nullptr;
}

void LogManager::SetLevel( LogLevel level ) {
    ASSERT( level >= LogLevel::LL_None && level <= LogLevel::LL_Error );
    mLogLevel = level;
}

LogLevel LogManager::GetLevel( ) const {
    return mLogLevel;
}

void LogManager::Write( int level, const String& message ) {
    ASSERT( pLogInstance && pLogInstance->mSpdLogger );
    pLogInstance->mSpdLogger->log( ToSpdLogLevel( level ), message.c_str( ) );
}

void LogManager::WriteRaw( const String& message, bool error ) {
    ASSERT( pLogInstance && pLogInstance->mSpdLogger );
    pLogInstance->mSpdLogger->log( ToSpdLogLevel( pLogInstance->mLogLevel ), message.c_str( ) );
}

String ToString( const char* function, const char* str, ... ) {
    const unsigned BUFFER_SIZE = 4096;
    char           buf[ BUFFER_SIZE ];

    va_list arglist;
    va_start( arglist, str );
    vsprintf_s( buf, BUFFER_SIZE, str, arglist );
    va_end( arglist );

    return String( "[" ) + String( function ) + "] " + String( buf );
}