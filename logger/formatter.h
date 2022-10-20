#pragma once
#include <string>
#include <algorithm>
#include <type_traits>
#include <sstream>

// Helper functions which do simple formatting.
// Sequence of {} in original string is treated as placeholder and replaced to passed argument.
// Example: the string "Abc {} {}" has two placeholders. Call format("Abc", 1, 2) will transform the string into "Abc 1 2".
namespace formatter
{
    template <typename T>
    std::string toString( T&& v )
    {
        std::stringstream buf;
        buf << v;
        return buf.str();
    }

    std::string format( std::string fmt )
    {
        return std::move( fmt );
    }

    std::string::iterator findNextPlaceHolder( std::string& fmt )
    {
        return std::adjacent_find( fmt.begin(), fmt.end(), []( char l, char r ) -> bool
            {
                return l == '{' && r == '}';
            } );
    }

    // Two formatImpl functions recursively iterate through parameter pack, convert
    // each parameter into string and replace placeholder with converted string.
    template <typename A>
    std::string& formatImpl( std::string& fmt, A&& a )
    {
        auto it = findNextPlaceHolder( fmt );
        const size_t placeholderLength = 2;
        if ( it != fmt.end() )
            fmt.replace( it, it + placeholderLength, toString(a));

        return fmt;
    }

    template <typename A, typename... Args>
    std::string& formatImpl( std::string& fmt, A&& first, Args&&... args )
    {
        auto it = findNextPlaceHolder( fmt );
        if ( it != fmt.end() )
        {
            const size_t placeholderLength = 2;
            fmt.replace( it, it + placeholderLength, toString(first) );
            return formatImpl( fmt, std::forward<Args>( args )... );
        }
        return fmt;
    }

    template <typename... Args>
    std::string format( std::string fmt, Args&&... args )
    {
        return std::move( formatImpl( fmt, std::forward<Args>( args )... ) );
    }
}