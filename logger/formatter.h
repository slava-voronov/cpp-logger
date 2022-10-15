#pragma once
#include <string>
#include <algorithm>
#include <type_traits>

template<typename T, typename = std::enable_if<std::is_integral<T>::value> >
std::string convert_to_string( T value )
{
	return std::to_string( value );
}

template<>
std::string convert_to_string( char value )
{
	return { value };
}

template<>
std::string convert_to_string( const char* value )
{
	return std::string{value};
}

template<>
std::string convert_to_string( std::string value )
{
	return std::move( value );
}

auto findNextPlaceHolder( std::string& fmt )
{
	return std::adjacent_find( fmt.begin(), fmt.end(), []( char l, char r ) -> bool
	{
	    return l == '{' && r == '}';
	} );
}

template <typename A>
std::string& format_impl( std::string& fmt, A a )
{
	auto it = findNextPlaceHolder( fmt );
	if ( it != fmt.end() )
		fmt.replace( it, it + 2, convert_to_string( a ) );

	return fmt;
}

template <typename A, typename... Args>
std::string& format_impl( std::string& fmt, A first, Args... args )
{
	auto it = findNextPlaceHolder( fmt );
	if ( it != fmt.end() )
	{
		fmt.replace( it, it + 2, convert_to_string( first ) );
		return format_impl( fmt, args... );
	}
	return fmt;
}

template <typename... Args>
std::string format( std::string fmt, Args... args )
{
	return std::move( format_impl( fmt, args... ) );
}
