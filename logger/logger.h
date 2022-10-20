#pragma once

#include <string>
#include <iostream>

#include "consumer.h"
#include "formatter.h"
#include "safequeue.h"

class Logger
{
    Logger();
    Logger( const Logger& ) = delete;
    Logger( Logger&& ) = delete;
public:
    ~Logger();

    inline static Logger& instance()
    {
        static Logger s_instance;
        return s_instance;
    }

    template<typename... Args>
    inline static void log_assert( std::string message, Args&&... args );

    template<typename... Args>
    inline static void debug( std::string message, Args&&... args );

    template<typename... Args>
    inline static void error( std::string message, Args&&... args );
    
    template<typename... Args>
    inline static void info( std::string message, Args&&... args );

    template<typename... Args>
    inline static void verbose( std::string message, Args&&... args );

    template<typename... Args>
    inline static void warn( std::string message, Args&&... args );

    enum LogLevel
    {
        eAssert = 0x1,
        eDebug = 0x2,
        eError = 0x4,
        eInfo = 0x8,
        eVerbose = 0x10,
        eWarning = 0x20,
        eAll = eAssert | eDebug | eError | eInfo | eVerbose | eWarning
    };
    inline static void attachLogFile( const std::string& filename, unsigned filter = Logger::LogLevel::eAll );

private:
    using LogMessageFormat = std::tuple<LogLevel, std::chrono::system_clock::time_point, std::string>;
    using Container = SafeQueue<LogMessageFormat>;

    inline static void doOutput( std::ostream& stream, const LogMessageFormat& logMessage );

    inline static std::string logLevelString( LogLevel level );

private:
    Container m_messages;
    Consumer<Container> m_consumer;
};

inline Logger::Logger(): m_consumer( m_messages )
{
    m_consumer.attachStreamOutput( std::cout );
    m_consumer.setPrinterFunction( &Logger::doOutput );

    m_consumer.start();
}

inline Logger::~Logger()
{
    m_consumer.stop();
}

template<typename... Args>
inline void Logger::log_assert( std::string message, Args&&... args )
{
    instance().m_messages.emplace( LogLevel::eAssert, std::chrono::system_clock::now(), 
        formatter::format( message, std::forward<Args>(args)... ) );
}

template<typename... Args>
inline void Logger::debug( std::string message, Args&&... args )
{
    instance().m_messages.emplace( LogLevel::eDebug, std::chrono::system_clock::now(),
        formatter::format( message, std::forward<Args>( args )... ) );
}

template<typename... Args>
inline void Logger::error( std::string message, Args&&... args )
{
    instance().m_messages.emplace( LogLevel::eError, std::chrono::system_clock::now(),
        formatter::format( message, std::forward<Args>( args )... ) );
}

template<typename... Args>
inline void Logger::info( std::string message, Args&&... args )
{
    instance().m_messages.emplace( LogLevel::eInfo, std::chrono::system_clock::now(),
        formatter::format( message, std::forward<Args>( args )... ) );
}

template<typename... Args>
inline void Logger::verbose( std::string message, Args&&... args )
{
    instance().m_messages.emplace( LogLevel::eVerbose, std::chrono::system_clock::now(),
        formatter::format( message, std::forward<Args>( args )... ) );
}

template<typename... Args>
void Logger::warn( std::string message, Args&&... args )
{
    instance().m_messages.emplace( LogLevel::eWarning, std::chrono::system_clock::now(),
        formatter::format( message, std::forward<Args>( args )... ) );
}

void Logger::attachLogFile( const std::string& filename, unsigned filter )
{
    instance().m_consumer.attachFileOutput( filename, [=]( const LogMessageFormat& logMessage ) -> bool
        {
            Logger::LogLevel level = std::get<0>( logMessage );
            return (filter & level) != 0;
        } );
}

inline void Logger::doOutput( std::ostream& stream, const LogMessageFormat& logMessage )
{
    const int maxLogLevelStringLen = 7;

    std::time_t timestamp = std::chrono::system_clock::to_time_t( std::get<1>( logMessage ) );
    std::tm formattedTimestamp = *std::localtime( &timestamp );

    stream << '[' << std::left << std::setw( maxLogLevelStringLen )
        << logLevelString( std::get<0>( logMessage ) )
        << std::put_time( &formattedTimestamp, "%c" ) << "]: "
        << std::get<2>( logMessage ) << std::endl;
}

inline std::string Logger::logLevelString( LogLevel level )
{
    switch ( level )
    {
    case LogLevel::eAssert:
        return "Assert";
    case LogLevel::eDebug:
        return "Debug";
    case LogLevel::eError:
        return "Error";
    case LogLevel::eInfo:
        return "Info";
    case LogLevel::eVerbose:
        return "Verbose";
    case LogLevel::eWarning:
        return "Warning";
    default:
        return "";
    }
}