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

bool insert(TokenNode & node , Token::const_iterator const & it , Token::const_iterator const & end)
{
	if (it != end)
		return insert(node.child_[*it] , it + 1 , end);
	bool new_token = false;
	if (node.token == false)
		node.token = new_token = true;
	return new_token;
}

bool exists(TokenNode const & node , Token::const_iterator const & it , Token::const_iterator const & end)
{
	if (it == end)
		return node.token;
	if (node.child_.find(*it) == node.child_.end())
		return false;
	return exists(node.child_.at(*it) , it + 1 , end);
}

size_t max_match(TokenNode const & node , Token::const_iterator const & it , Token::const_iterator const & end , size_t index)
{
	if (it == end ||
		node.child_.find(*it) == node.child_.cend())
		return node.token ? index : 0;
	size_t result = max_match(node.child_.at(*it) , it + 1 , end , index + 1);
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
		posting_ = make_shared<PostingList>();
		posting_->push_back(Posting(document));
	}
	else if (posting_->back().document != document)
		posting_->push_back(Posting(document));
	else
		++posting_->back().frequency;
	return new_token;
}

std::shared_ptr<PostingList const> const & InvertedNode::posting_list() const
{
	return posting_;
}

bool insert(InvertedNode & node , unsigned long long document , 
			Token::const_iterator const & it , Token::const_iterator const & end)
{
	if (it == end)
		return node.insert(document);
	else
		return insert(node.child_[*it] , document , it + 1 , end);
}

InvertedNode const * retrieve(InvertedNode const & node , Token::const_iterator const & it , Token::const_iterator const & end)
{
	if (it == end)
		return (node.posting_->empty()) ? nullptr : &node;
	if (node.child_.find(*it) == node.child_.end())
		return nullptr;
	return retrieve(node.child_.at(*it) , it + 1 , end);
}