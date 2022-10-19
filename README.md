# cpp-logger

Assignment for Rightware.

**Install**

---

Headers only logger. No installation is required. Copy [logger](https://github.com/slava-voronov/cpp-logger/tree/main/logger) folder to your build tree and use a C++11 compiler or hihger.

**Features**

---

* Headers only.
* Several log levels.
* Multithreaded logging.
* Logging into files with filters.
* Support for logging custom types.

**Usage sample**

---

Basic usage

```
#include "logger.h"

int main()
{
    // No special initialization is required.
    Logger::info( "Hello world!" );
    Logger::warn( "Some warinig message." );
    Logger::error( "Something went wrong!" );
}
```

Logging into file

```
#include "logger.h" 

int main()
{
    // Attaching info.log file to store only information log messages.
    Logger::attachLogFile( "info.log", Logger::LogLevel::eInfo );
    // Attaching info.log file to store only error log messages.
    Logger::attachLogFile( "error.log", Logger::LogLevel::eError );

    // Console will still display all log messages.

    Logger::info( "Hello world!" );
    Logger::warn( "Some warinig message." );
    Logger::error( "Something went wrong!" ); }
}
```

Logging with placeholders
Logger supports simple formatting. A string message may have {} as placeholder. Replacement for each {} is taken from function parameters.

```
#include "logger.h"

int main() 
{
    std::string month = "October";
    int day = 19;
    std::string dayOfWeek = "Wednesday";

   // This call will output the following message:
   // [Info 10/19/22 14:22:45]: Daily log from October-19, Wednesday
   Logger::info( "Daily log from {}-{}, {}", month, day, dayOfWeek ); 
}
```

Logging for custom types
Objects of custom types may be logged if an operator<< into std::ostream is provided.

```
#include "logger.h"

// Custom type which should be logged.
struct MyType
{
    int id;
    std::string description;
};

// Operator for output MyType into std::ostream
std::ostream& operator<<( std::ostream& os, const MyType& var )
{
    os << "[id:" << var.id << " description:" << var.description << "]";
    return os;
}

int main() 
{ 
    MyType value{ 0, "null-object" };

    // This call will output the following message: 
    // [Info   10/19/22 14:59:42]: An example of logging custom types. This is MyType object: [id:0 description:null-object]
    Logger::info( "An example of logging custom types. This is MyType object: {}", value );
}
```

Logging from multiple threads
Multuthreaded logging is supported out-of-box. No user action is required.

```
int main()
{
    Logger::info( "Start logging example" );

    // Create three threads which emit messages in parallel
    std::thread t1([&amp;]() {
        for ( int i = 0; i < 100; i++ )	{
            Logger::warn( "thread 1" );
            std::this_thread::sleep_for( std::chrono::milliseconds( 30 ) );
	}});

    std::thread t2([&amp;]() {
        for ( int i = 0; i < 100; i++ )	{
            Logger::error( "thread 2" );
	    std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
        }});

    std::thread t3([&amp;]() {
        for ( int i = 0; i < 100; i++ )	{
            Logger::info( "thread 3" );
            std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
	}});
    // Wait for all thread finish.
    t1.join();
    t2.join();
    t3.join();

    return 0;
}
```

The provided code will print the following into console:

```

    [Info   10/19/22 15:08:01]: thread 3
    [Warnig 10/19/22 15:08:01]: thread 1
    [Warnig 10/19/22 15:08:01]: thread 1
    [Info   10/19/22 15:08:01]: thread 3
    [Error  10/19/22 15:08:01]: thread 2
```
