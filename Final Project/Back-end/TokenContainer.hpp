#pragma once

#include "Node.hpp"

#include <stack>

class TokenContainer;

class TokenWrapper
{
public:

	TokenNode * node;

	bool expanded;

	TokenChildren::iterator it , end;

	TokenWrapper(TokenNode * node);
};


class TokenIterator
{
	friend TokenContainer;
public:
	
	TokenIterator & operator++();

	TokenIterator operator++(int);

	bool end() const;

	Token const & token() const;

	std::wstring to_string() const;

private:
	
	std::stack<TokenWrapper> record;

	Token data;

	void push();
};

class TokenContainer
{	//	Trie of a token container
	//	Each node equivalent to a word

	
public:

	//	Insert a token in the list
	void insert(Token const & token);
	void insert(Token::const_iterator & it , Token::const_iterator const & end);

	//	Check if a token in the list
	bool exists(Token const & token) const;
	bool exists(Token::const_iterator & it , Token::const_iterator const & end) const;

	//	Maximum matching
	size_t max_match(Token const & token) const;
	size_t max_match(Token::const_iterator & it , Token::const_iterator const & end) const;

	//	Size of the token container
	size_t size() const;

	TokenIterator iterator();
private:
	
	//	Root of trie
	TokenNode root;

	size_t container_size;
};