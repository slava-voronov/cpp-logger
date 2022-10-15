#include <string>
#include <iostream>
#include <chrono>
#include <memory>
#include <algorithm>
#include <sstream>

#include "logger.h"
#include "formatter.h"

#include <windows.h>

struct user_type
{
	std::string tag;
	std::chrono::system_clock::time_point time;
};

template<>
std::string convert_to_string( user_type& type )
{
	std::stringstream buf;
	buf << type.tag << "time: ";
	return buf.str();
}

int main()
{
	using namespace std::chrono_literals;

	Logger logger;

	char buffer[MAX_PATH];
	DWORD length = GetModuleFileNameA( NULL, buffer, MAX_PATH );
	std::string exeFullPath( buffer );
	auto pos = exeFullPath.find_last_of('\\');
	auto folder = exeFullPath.substr( 0, pos );

	logger.addOutput( folder + "\\test.log");

	logger.warn( "info" );
	logger.error( "info" );
	logger.info( "info" );

	std::string fmt{"abc{{}} {} bds:{} {}"};
	std::string res = format( fmt, 10, 16.0f, 'a', "zzz");

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