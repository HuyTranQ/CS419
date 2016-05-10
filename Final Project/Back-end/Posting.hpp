#pragma once

#include <climits>

class Posting
{
public:
	unsigned long long document , frequency;
	Posting()
		: document(ULLONG_MAX) , frequency(ULLONG_MAX)
	{}

	Posting(unsigned long long document)
		: document(document) , frequency(1)
	{}

	Posting(unsigned long long document , unsigned long long frequency)
		: document(document) , frequency(frequency)
	{}
};