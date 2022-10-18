#include "../logger/formatter.h"
#include <gtest/gtest.h>

TEST( FormatterTests, TestFundamentalTypes )
{
	std::string expected = "Unsigned int: -1 Float: 1 Char: a";
	std::string actual = formatter::format( "Unsigned int: {} Float: {} Char: {}", -1, 1.0f, 'a' );
	ASSERT_EQ( expected, actual );
}

TEST( FormatterTests, TestStringTypes )
{
	std::string std{"this is std::string"};

	std::string expected = "std::string: \"this is std::string\" c-style string:\"and this is c-style string\"";
	std::string actual = formatter::format( "std::string: \"{}\" c-style string:\"{}\"", std, "and this is c-style string");
	ASSERT_EQ( expected, actual );
}

struct MyType
{
	int id;
	std::string description;
};

std::ostream& operator<<( std::ostream& os, const MyType& var )
{
	os << "[id:" << var.id << " description:" << var.description << "]";
	return os;
}

TEST( FormatterTests, TestCustomTypes )
{
	MyType value{ 0, "null-object" };

	std::string expected = "Custom type output: [id:0 description:null-object]";
	std::string actual = formatter::format( "Custom type output: {}", value );
	ASSERT_EQ( expected, actual );
}

int main( int argc, char** argv )
{
	::testing::InitGoogleTest( &argc, argv );
	return RUN_ALL_TESTS();
}