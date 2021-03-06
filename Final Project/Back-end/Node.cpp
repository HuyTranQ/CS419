#include "Node.hpp"

using std::make_shared;

/*
	TOKEN NODE
*/

bool TokenNode::is_token() const
{
	return token;
}

TokenNode::TokenNode()
	: token(false)
{
}

bool insert(TokenNode & node , Token::const_iterator & it , Token::const_iterator const & end)
{
	if (it != end)
	{
		auto & value = *it;
		return insert(node.child_[value] , ++it , end);
	}
	bool new_token = false;
	if (node.token == false)
		node.token = new_token = true;
	return new_token;
}

bool exists(TokenNode const & node , Token::const_iterator & it , Token::const_iterator const & end)
{
	if (it == end)
		return node.token;
	auto & value = *it;
	if (node.child_.find(value) == node.child_.end())
		return false;
	return exists(node.child_.at(value) , ++it , end);
}

size_t max_match(TokenNode const & node , Token::const_iterator & it , Token::const_iterator const & end , size_t index)
{
	if (it == end)
		return node.token ? index : 0;
	auto & value = *it;
	if (node.child_.find(value) == node.child_.cend())
		return node.token ? index : 0;
	size_t result = max_match(node.child_.at(value) , ++it , end , index + 1);
	if (result > 0)
		return result;
	else
		return node.token ? index : 0;
}



/*
	INVERTED NODE
*/

InvertedNode::InvertedNode()
	: posting_(nullptr)
{
}

bool InvertedNode::insert(unsigned long long document)
{
	bool new_token = false;
	if (posting_ == nullptr)
	{
		new_token = true;
		posting_ = new PostingList();
		posting_->push_back(Posting(document));
	}
	else if (posting_->back().document != document)
		posting_->push_back(Posting(document));
	else
		++posting_->back().frequency;
	return new_token;
}

InvertedNode::~InvertedNode()
{
	delete posting_;
}

PostingList const * InvertedNode::posting_list() const
{
	return posting_;
}

bool insert(InvertedNode & node , unsigned long long document , 
			Token::const_iterator & it , Token::const_iterator const & end)
{
	if (it == end)
		return node.insert(document);
	else
	{
		auto & value = *it;
		return insert(node.child_[value] , document , ++it , end);
	}
}

InvertedNode const * retrieve(InvertedNode const & node , Token::const_iterator & it , Token::const_iterator const & end)
{
	if (it == end)
		return (node.posting_ == nullptr || node.posting_->empty()) ? nullptr : &node;
	auto & value = *it;
	if (node.child_.find(value) == node.child_.end())
		return nullptr;
	return retrieve(node.child_.at(value) , ++it , end);
}