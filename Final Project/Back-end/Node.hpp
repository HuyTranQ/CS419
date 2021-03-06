#pragma once

#include "Posting.hpp"

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <deque>
#include <list>

class TokenNode;
class TokenIterator;
class TokenWrapper;

using Token = std::deque<std::wstring>;
using TokenChildren = std::unordered_map<std::wstring , TokenNode>;

class TokenNode
{

	//	Friendships
	friend bool insert(TokenNode & node , Token::const_iterator & it , Token::const_iterator const & end);
	friend bool exists(TokenNode const & node , Token::const_iterator & it , Token::const_iterator const & end);
	friend size_t max_match(TokenNode const & node , Token::const_iterator & it , Token::const_iterator const & end , size_t index);

	friend class TokenIterator;
	friend class TokenWrapper;
	
public:
	
	//	Default Constructor
	TokenNode();
	
	//	If this token node is at the end of a token => true
	bool is_token() const;

private:

	bool token;

	TokenChildren child_;
};


class InvertedNode;
class InvertedIterator;

using PostingList = std::vector<Posting>;
using InvertedChildren = std::unordered_map<std::wstring , InvertedNode>;

class InvertedNode
{
	friend class InvertedIterator;
	friend bool insert(InvertedNode & node , unsigned long long document ,
					   Token::const_iterator & it , Token::const_iterator const & end);
	friend InvertedNode const * retrieve(InvertedNode const & node , Token::const_iterator & it , Token::const_iterator const & end);
public:

	InvertedNode();
	~InvertedNode();

	//	Insert a document to current posting list
	bool insert(unsigned long long document);

	//	Get (constant) posting list
	PostingList const * posting_list() const;

private:

	//	Shared pointer to Posting List
	PostingList * posting_;

	//	Children
	InvertedChildren child_;

};