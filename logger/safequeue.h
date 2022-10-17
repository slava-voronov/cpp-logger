#pragma once

#include <queue>
#include <mutex>

template<typename T>
class SafeQueue
{
public:
	using value_type = typename T;
	void push( const T& value )
	{
		std::lock_guard<std::mutex> lock( m_mutex );
		m_queue.push_back( value );
	}

	void push( T&& value )
	{
		std::lock_guard<std::mutex> lock( m_mutex );
		m_queue.push_back( value );
	}

	template<typename... Args>
	void emplace( Args&&... args )
	{
		std::lock_guard<std::mutex> lock( m_mutex );
		m_queue.emplace( std::forward<Args>( args )... );
	}

	bool pop( T& record )
	{
		std::lock_guard<std::mutex> lock( m_mutex );
		if ( !m_queue.empty() )
		{
			record = std::move( m_queue.front() );
			m_queue.pop();
			return true;
		}
		return false;
	}

	bool empty() const
	{
		return m_queue.empty();
	}

private:
	std::mutex m_mutex;
	std::queue<T> m_queue;
};
