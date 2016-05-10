#include "TokenContainer.hpp"

using std::wstring;

/*
	TOKEN ITERATOR
*/

TokenWrapper::TokenWrapper(TokenNode * node)
	: node(node) , expanded(false)
{
}

TokenIterator & TokenIterator::operator++()
{
	if (record.empty())
		return (*this);
	TokenWrapper & top = record.top();
	if (!top.expanded)
	{
		if (top.node->child_.empty())
		{
			record.pop();
			return operator++();
		}
		else
		{
			top.it = top.node->child_.begin();
			top.end = top.node->child_.end();
			top.expanded = true;
			push();
			if (!record.top().node->token)
				return operator++();
		}
	}
	else
	{
		++top.it;
		data.pop_back();
		if (top.it == top.end)
		{
			record.pop();
			return operator++();
		}
		push();
		if (!record.top().node->token)
			return operator++();
	}
	return (*this);
}

TokenIterator TokenIterator::operator++(int)
{
	TokenIterator previous;
	operator++();
	return previous;
}

bool TokenIterator::end() const
{
	return record.empty();
}

Token const & TokenIterator::token() const
{
	return data;
}

std::wstring TokenIterator::to_string() const
{
	wstring result;
	size_t length = data.size();
	if (length > 0)
	{
		size_t i = 0;
		while (true)
		{
			result += data[i];
			++i;
			if (i == length)
				break;
			result += L' ';
		}
	}
	return result;
}

void TokenIterator::push()
{
	TokenWrapper & top = record.top();
	data.push_back(top.it->first);
	record.push(TokenWrapper(&top.it->second));
}






/*
	TOKEN CONTAINER
*/

void TokenContainer::insert(Token const & token)
{
	insert(token.cbegin() , token.cend());
}

void TokenContainer::insert(Token::const_iterator const & it , Token::const_iterator const & end)
{
	if (::insert(root , it , end))
		++container_size;
}

bool TokenContainer::exists(Token const & token) const
{
	return exists(token.cbegin() , token.cend());
}

bool TokenContainer::exists(Token::const_iterator const & it , Token::const_iterator const & end) const
{
	return ::exists(root , it , end);
}

size_t TokenContainer::max_match(Token const & token) const
{
	return max_match(token.cbegin() , token.cend());
}

size_t TokenContainer::max_match(Token::const_iterator const & it , Token::const_iterator const & end) const
{
	return ::max_match(root , it , end , 0);
}

size_t TokenContainer::size() const
{
	return container_size;
}

TokenIterator TokenContainer::iterator()
{
	TokenIterator iterator;
	iterator.record.push(TokenWrapper(&root));
	++iterator;
	return iterator;
}