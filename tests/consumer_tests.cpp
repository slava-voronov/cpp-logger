#include "../logger/consumer.h"
#include <gtest/gtest.h>

#include <queue>

template <typename T>
struct TestSafeQueue
{
	using value_type = T;
	void push( const T& value )
	{
		data.push( value );
	}

	void push( T&& value )
	{
		data.push( value );
	}

	template<typename... Args>
	void emplace( Args&&... args )
	{
		data.emplace( std::forward<Args>( args )... );
	}

	bool pop( T& record )
	{
		if ( !data.empty() )
		{
			record = std::move( data.front() );
			data.pop();
			return true;
		}
		return false;
	}

	bool empty() const
	{
		return data.empty();
	}

private:
	std::queue<T> data;
};

TEST( ConsumerTests, TestStringOutput )
{
	TestSafeQueue<std::pair<int, std::string>> container;

	container.push( std::make_pair( 1, "1" ) );
	container.push( std::make_pair( 2, "2" ) );
	container.push( std::make_pair( 3, "3" ) );

	Ñonsumer<TestSafeQueue<std::pair<int, std::string>>> worker( container );

	std::stringstream output;
	worker.attachStreamOutput( output );
	worker.setPrinterFunction( []( std::ostream& output, const std::pair<int, std::string>& record )
		{
			output << "id: " << record.first << " value: " << record.second << std::endl;
		} );

	std::string expected = "id: 1 value: 1\nid: 2 value: 2\nid: 3 value: 3\n";

	worker.start();
	worker.stop();

	std::string actual = output.str();
	ASSERT_EQ( expected, actual );
}

TEST( ConsumerTests, TestFilteredOutput )
{
	TestSafeQueue<std::pair<int, std::string>> container;

	container.push( std::make_pair( 1, "1" ) );
	container.push( std::make_pair( 2, "2" ) );
	container.push( std::make_pair( 3, "3" ) );
	container.push( std::make_pair( 4, "4" ) );
	container.push( std::make_pair( 5, "5" ) );
	container.push( std::make_pair( 6, "6" ) );

	Ñonsumer<TestSafeQueue<std::pair<int, std::string>>> worker( container );

	worker.setPrinterFunction( []( std::ostream& output, const std::pair<int, std::string>& record )
		{
			output << "id: " << record.first << " value: " << record.second << std::endl;
		} );

	std::stringstream outputEven;
	worker.attachStreamOutput( outputEven, []( const std::pair<int, std::string>& record ) -> bool 
		{
			return record.first % 2 == 0;
		} );

	std::stringstream outputOdd;
	worker.attachStreamOutput( outputOdd, []( const std::pair<int, std::string>& record ) -> bool
		{
			return record.first % 2 != 0;
		} );

	std::stringstream outputAll;
	worker.attachStreamOutput( outputAll );

	worker.start();
	worker.stop();

	std::string expectedEven = "id: 2 value: 2\nid: 4 value: 4\nid: 6 value: 6\n";
	std::string actualEven = outputEven.str();
	ASSERT_EQ( expectedEven, actualEven );

	std::string expectedOdd = "id: 1 value: 1\nid: 3 value: 3\nid: 5 value: 5\n";
	std::string actualOdd = outputOdd.str();
	ASSERT_EQ( expectedOdd, actualOdd );

	std::string expectedAll = "id: 1 value: 1\nid: 2 value: 2\nid: 3 value: 3\nid: 4 value: 4\nid: 5 value: 5\nid: 6 value: 6\n";
	std::string actualAll = outputAll.str();
	ASSERT_EQ( expectedAll, actualAll );
}