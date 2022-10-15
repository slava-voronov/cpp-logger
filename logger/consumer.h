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
	Ñonsumer( T& container ) :
		m_container( container )
	{
		m_targets.push_back( []() -> std::ostream& 
		{
			return std::cout;
		});
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

	void addTarget( const std::string& filename )
	{
		auto fileStream = std::make_shared<std::fstream>( filename, std::ios::out );
		m_targets.push_back( [=]() -> std::ostream& { return fileStream.operator*(); } );
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
				for ( auto& target : m_targets )
					target() << record.first << ': ' << record.second << std::endl;
			}
		}
	}

private:
	T& m_container;
	std::unique_ptr<std::thread> m_worker;
	std::atomic_bool m_stopped = false;

	std::function<void( const typename T::value_type&, std::ostream& )> m_printer;

	std::vector<std::function<std::ostream&()>> m_targets;
};
