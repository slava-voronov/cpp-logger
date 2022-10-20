#pragma once
#include <string>
#include <algorithm>
#include <type_traits>
#include <sstream>

namespace formatter
{
	template <typename T>
	std::string toString( T&& v )
	{
		std::stringstream buf;
		buf << v;
		return buf.str();
	}

	std::string::iterator findNextPlaceHolder( std::string& fmt )
	{
		return std::adjacent_find( fmt.begin(), fmt.end(), []( char l, char r ) -> bool
			{
				return l == '{' && r == '}';
			} );
	}

	template <typename A>
	std::string& formatImpl( std::string& fmt, A&& a )
	{
		auto it = findNextPlaceHolder( fmt );
		if ( it != fmt.end() )
			fmt.replace( it, it + 2, toString(a));

		return fmt;
	}

	template <typename A, typename... Args>
	std::string& formatImpl( std::string& fmt, A&& first, Args&&... args )
	{
		auto it = findNextPlaceHolder( fmt );
		if ( it != fmt.end() )
		{
			fmt.replace( it, it + 2, toString(first) );
			return formatImpl( fmt, std::forward<Args>( args )... );
		}
		return fmt;
	}

	std::string format( std::string fmt )
	{
		return std::move( fmt );
	}

	template <typename... Args>
	std::string format( std::string fmt, Args&&... args )
	{
		return std::move( formatImpl( fmt, std::forward<Args>( args )... ) );
	}
}