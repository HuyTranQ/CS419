#pragma once

#include "Node.hpp"

#include <utility>
#include <stack>

class InvertedIterator;
class InvertedIndex;

class InvertedWrapper
{
	friend InvertedIterator;
public:

	InvertedWrapper(InvertedNode * node);

private:
	InvertedNode * node;

	bool expanded;

	InvertedChildren::iterator it , end;
};

class InvertedIterator
{
	friend InvertedIndex;
public:

	InvertedIterator & operator++();

	InvertedIterator operator++(int);

	PostingList & posting_list();
	PostingList const & posting_list() const;

	Token const & token() const;

	std::wstring to_string() const;

	bool end() const;

private:

	std::stack<InvertedWrapper> record;

	Token data;

	void push();
};

class InvertedIndex
{
public:

	void insert(Token const & token , unsigned long long document);
	void insert(Token::const_iterator & it , Token::const_iterator const & end ,
				unsigned long long document);

	InvertedNode * operator[](Token const & token);

	InvertedNode const * operator[](Token const & token) const;

	InvertedNode * at(Token::const_iterator & it ,
					  Token::const_iterator const & end);

	InvertedNode const * at(Token::const_iterator & it ,
							Token::const_iterator const & end) const;

	InvertedIterator iterator();

	size_t size() const;
private:

	InvertedNode root;

	size_t inverted_size;
};