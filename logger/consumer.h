#pragma once

#include <thread>
#include <memory>
#include <atomic>
#include <vector>
#include <ostream>
#include <fstream>
#include <functional>

template<typename T>
class Ñonsumer
{
public:
	using stream_type = std::function<std::ostream&(void)>;
	using filter_type = std::function<bool( const typename T::value_type& )>;

	Ñonsumer( T& container ) :
		m_container( container )
	{
	}

	void start()
	{
		m_worker.reset( new std::thread( &Ñonsumer<T>::run, this ) );
	}

	void stop()
	{
		m_stopped = true;
		m_worker->join();
	}

	void setPrinterFunction( std::function<void( std::ostream&, const typename T::value_type& )> printer )
	{
		m_printer = printer;
	}

	void attachStreamOutput( std::ostream& output, filter_type filter = nullptr )
	{
		m_targets.push_back( std::pair<stream_type, filter_type>( [&]() -> std::ostream&
			{
				return output;
			}, filter ) );
	}

	void attachFileOutput( const std::string& filename, filter_type filter = nullptr )
	{
		auto fileStream = std::make_shared<std::fstream>( filename, std::ios::out );
		stream_type stream = [=]() -> std::ostream& { return fileStream.operator*(); };
		m_targets.push_back( std::pair<stream_type, filter_type>( [=]() -> std::ostream& { return fileStream.operator*(); }, filter ) );
	}

private:
	void run()
	{
		while ( !m_stopped || !m_container.empty() )
		{
			T::value_type record;
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
	std::atomic_bool m_stopped = false;

	std::function<void( std::ostream&, const typename T::value_type& )> m_printer;

	std::vector<std::pair<stream_type, filter_type>> m_targets;
};
