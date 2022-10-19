#include <string>
#include <chrono>

#include "logger.h"

// The sample supports file logginf only on Windows platforms. As std library does not provide operations on paths in C++11.
#ifdef _WINDOWS
    #include <windows.h>
#endif

int main()
{
	using namespace std::chrono_literals;

#ifdef _WINDOWS
	char buffer[MAX_PATH];
	DWORD length = GetModuleFileNameA( NULL, buffer, MAX_PATH );
	std::string exeFullPath( buffer );
	auto pos = exeFullPath.find_last_of( '\\' );
	auto folder = exeFullPath.substr( 0, pos );

	// Attach log file. Enable only info and warning messages. Messages with other log levels will be filtered out.
	// File is created in the folder with executable.
	Logger::attachLogFile( folder + "\\test.log", Logger::LogLevel::eInfo | Logger::LogLevel::eWarning );
#endif // _WINDOWS

	// Start logging.
	Logger::info( "Start logging example" );

	// Thread one will emit warning logs.
	std::thread t1( [&]()
		{
			for ( int i = 0; i < 100; i++ )
			{
				Logger::warn( "thread 1" );
				std::this_thread::sleep_for( 30ms );
			}
		} );

	// Thread two will emit error logs.
	std::thread t2( [&]()
		{
			for ( int i = 0; i < 100; i++ )
			{
				Logger::error( "thread 2" );
				std::this_thread::sleep_for( 100ms );
			}
		} );

	// Thread two will emit info logs.
	std::thread t3( [&]()
		{
			for ( int i = 0; i < 100; i++ )
			{
				Logger::info( "thread 3" );
				std::this_thread::sleep_for( 50ms );
			}
		} );

	t1.join();
	t2.join();
	t3.join();

	return 0;
}