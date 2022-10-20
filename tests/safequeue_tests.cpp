#include "../logger/safequeue.h"
#include <gtest/gtest.h>

#include <thread>

TEST( SafeQueueTests, TestThreading )
{
    SafeQueue<int> queue;

    std::vector<int> threadOneSource( 100, 1 );
    std::vector<int> threadTwoSource( 100, 2 );
    std::vector<int> threadThreeSource( 100, 3 );

    std::vector<int> result;

    std::atomic_bool threadOneDone{ false };
    std::atomic_bool threadTwoDone{ false };
    std::atomic_bool threadThreeDone{ false };

    std::thread thread1([&]()
    {
        for ( auto i : threadOneSource )
            queue.push( i );
        threadOneDone.store( true );    
    });

    std::thread thread2([&]()
    {
        for ( auto i : threadTwoSource )
            queue.push( i );
        threadTwoDone.store( true );    
    });

    std::thread thread3([&]()
    {
        for ( auto i : threadThreeSource )
            queue.push( i );
        threadThreeDone.store( true );    
    });

    std::thread worker([&]()
    {
        while ( !threadOneDone.load() || !threadTwoDone.load() || !threadThreeDone.load() || !queue.empty() )
        {
            int i = 0;
            if ( queue.pop( i ) )
                result.push_back( i );
        }
    });

    thread1.join();
    thread2.join();
    thread3.join();
    worker.join();

    size_t expectedResultSize = 300;
    size_t actualResultSize = result.size();
    ASSERT_EQ( expectedResultSize, actualResultSize );

    size_t expectedOneCount = 100;
    size_t actualOneCount = std::count_if(result.begin(), result.end(), [](int i) {return i == 1;});
    ASSERT_EQ( expectedOneCount, actualOneCount );

    size_t expectedTwoCount = 100;
    size_t actualTwoCount = std::count_if(result.begin(), result.end(), [](int i) {return i == 2;});
    ASSERT_EQ( expectedTwoCount, actualTwoCount );

    size_t expectedThreeCount = 100;
    size_t actualThreeCount = std::count_if(result.begin(), result.end(), [](int i) {return i == 3;});
    ASSERT_EQ( expectedThreeCount, actualThreeCount );
}