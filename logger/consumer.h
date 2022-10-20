#pragma once

#include <thread>
#include <memory>
#include <atomic>
#include <vector>
#include <ostream>
#include <fstream>
#include <functional>

// Helper class which extracts log messages from the container and does output to targets.
// Target is a combination of output stream and filter functions.
// Output stream is represented by lambda function which returns a reference to std::ostream.
template<typename T>
class Consumer
{
public:
	using stream_type = std::function<std::ostream&(void)>;
	using filter_type = std::function<bool( const typename T::value_type& )>;
    using target_type = std::pair<stream_type, filter_type>;
    using printer_type = std::function<void( std::ostream&, const typename T::value_type& )>;

	Consumer( T& container ) :
		m_container( container ), m_stopped( false )
	{
	}

	void start()
	{
		m_worker.reset( new std::thread( &Consumer<T>::run, this ) );
	}

	void stop()
	{
		m_stopped = true;
		m_worker->join();
	}

	void setPrinterFunction( printer_type printer )
	{
		m_printer = printer;
	}

    // Output stream is passed by value and is owned by caller. Lambda captures it by reference.
	void attachStreamOutput( std::ostream& output, filter_type filter = nullptr )
	{
		m_targets.push_back( std::pair<stream_type, filter_type>( [&]() -> std::ostream& { return output; }, filter ) );
	}

    // Output file with the specified filename will be created.
	void attachFileOutput( const std::string& filename, filter_type filter = nullptr )
	{
        // C++ 11 does not allow to move into lambda. Shared_ptr for std::fstream is created and captured by value instead.
		auto fileStream = std::make_shared<std::fstream>( filename, std::ios::out );
		stream_type stream = [=]() -> std::ostream& { return fileStream.operator*(); };
		m_targets.push_back( std::pair<stream_type, filter_type>( [=]() -> std::ostream& { return fileStream.operator*(); }, filter ) );
	}

private:
    // Worker method. Extracts log messages from the container one by one and prints to all attached targets.
	void run()
	{
		while ( !m_stopped || !m_container.empty() )
		{
			typename T::value_type record;
			auto succeed = m_container.pop( record );
			if ( succeed )
			{
				if ( m_printer )
				{
					for ( auto& target : m_targets )
					{
						auto getStreamFunc = target.first;
						auto filterFunc = target.second;
						if ( filterFunc )
						{
                            // Apply filtering
							if ( filterFunc( record ) )
							    m_printer( getStreamFunc(), record );
						}
						else
						{
							m_printer( getStreamFunc(), record );
						}
					}
				}
			}
		}
	}

private:
	T& m_container;
	std::unique_ptr<std::thread> m_worker;
	std::atomic_bool m_stopped;

	printer_type m_printer;

	std::vector<target_type> m_targets;
};
