#pragma once

#include <queue>
#include <string>
#include <iostream>
#include <atomic>

#include "consumer.h"
#include "safequeue.h"
#include "producer.h"

class Logger
{
	Logger( const Logger& ) = delete;
	Logger( Logger&& ) = delete;
public:
	Logger() : m_producer( m_messages ), m_consumer( m_messages )
	{
		m_consumer.start();
	}

	~Logger()
	{
		m_consumer.stop();
	}

	inline void assert( std::string message )
	{
		m_producer.output().emplace( "error:", std::move( message ) );
	}

	inline void debug( std::string message )
	{
		m_producer.output().emplace( "error:", std::move( message ) );
	}

	inline void error( std::string message )
	{
		m_producer.output().emplace( "error:", std::move( message ) );
	}
	
	inline void info( std::string message )
	{
		m_producer.output().emplace( "info:", std::move( message ) );
	}

	inline void verbose( std::string message )
	{
		m_producer.output().emplace( "info:", std::move( message ) );
	}

	void warn( std::string message )
	{
		m_producer.output().emplace( "warning:", std::move( message ) );
	}

	void addOutput( const std::string& filename )
	{
		m_consumer.addTarget( filename );
	}

private:
	inline void logMessage( int level, std::string&& message )
	{
		m_producer.output().emplace( "warning:", message );
	}

private:
	using Container = SafeQueue<std::pair<std::string, std::string>>;
	Container m_messages;
	Producer<Container> m_producer;
	Ñonsumer<Container> m_consumer;
};