#include <string>
#include <iostream>
#include <chrono>
#include <memory>
#include <algorithm>
#include <sstream>

#include "logger.h"
#include "formatter.h"

#include <windows.h>

template <typename T>
struct TestSafeQueue
{
	using value_type = T;
	void push( const T& value )
	{
		data.push( value );
	}

	void push( T&& value )
	{
		data.push( value );
	}

	template<typename... Args>
	void emplace( Args&&... args )
	{
		data.emplace( std::forward<Args>( args )... );
	}

	bool pop( T& record )
	{
		if ( !data.empty() )
		{
			record = std::move( data.front() );
			data.pop();
			return true;
		}
		return false;
	}

	bool empty() const
	{
		return data.empty();
	}

private:
	std::queue<T> data;
};


int main()
{
	using namespace std::chrono_literals;

	Logger logger;

	char buffer[MAX_PATH];
	DWORD length = GetModuleFileNameA( NULL, buffer, MAX_PATH );
	std::string exeFullPath( buffer );
	auto pos = exeFullPath.find_last_of('\\');
	auto folder = exeFullPath.substr( 0, pos );

	logger.addLoggingFile( folder + "\\test.log", Logger::eInfo );

	logger.warn( "info" );
	logger.error( "info" );
	logger.info( "info" );

	std::string fmt{"abc{{}} {} bds:{} {}"};
	std::string res = formatter::format( fmt, 16.0f, 'a', "zzz");

	std::thread t1( [&]()
		{
			for ( int i = 0; i < 100; i++ )
			{
				logger.warn( "t1" );
				std::this_thread::sleep_for( 30ms );
			}
		} );

	std::thread t2( [&]()
		{
			for ( int i = 0; i < 100; i++ )
			{
				logger.error( "t2" );
				std::this_thread::sleep_for( 100ms );
			}
		} );

	std::thread t3( [&]()
		{
			for ( int i = 0; i < 100; i++ )
			{
				logger.info( "t3" );
				std::this_thread::sleep_for( 50ms );
			}
		} );

	t1.join();
	t2.join();
	t3.join();

	return 0;
}