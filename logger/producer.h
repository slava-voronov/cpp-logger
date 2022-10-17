#pragma once

#include "consumer.h"

template<typename T>
class Producer
{
public:
	Producer( T& container ) : m_output(container)
	{
	}

	~Producer()
	{
	}

	T& output()
	{
		return m_output;
	}

private:
	T& m_output;
};
