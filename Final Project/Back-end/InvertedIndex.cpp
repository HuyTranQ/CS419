#include "InvertedIndex.hpp"

using std::wstring;

InvertedWrapper::InvertedWrapper(InvertedNode * node)
	: node(node) , expanded(false)
{
}

InvertedIterator & InvertedIterator::operator++()
{
	if (record.empty())
		return (*this);
	InvertedWrapper & top = record.top();
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
			if (nullptr == record.top().node->posting_)
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
		if (nullptr == record.top().node->posting_)
			return operator++();
	}
	return (*this);
}

void InvertedIterator::push()
{
	InvertedWrapper & top = record.top();
	data.push_back(top.it->first);
	record.push(&top.it->second);
}

InvertedIterator InvertedIterator::operator++(int)
{
	InvertedIterator previous;
	operator++();
	return previous;
}

PostingList & InvertedIterator::posting_list()
{
	return const_cast<PostingList &>(static_cast<InvertedIterator const &>(*this).posting_list());
}

PostingList const & InvertedIterator::posting_list() const
{
	return *record.top().node->posting_;
}

bool InvertedIterator::end() const
{
	return record.empty();
}

Token const & InvertedIterator::token() const
{
	return data;
}

std::wstring InvertedIterator::to_string() const
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

InvertedIterator InvertedIndex::iterator()
{
	InvertedIterator result;
	result.record.push(InvertedWrapper(&root));
	++result;
	return result;
}

void InvertedIndex::insert(Token const & token , unsigned long long document)
{
	insert(token.cbegin() , token.cend() , document);
}

void InvertedIndex::insert(Token::const_iterator & it , Token::const_iterator const & end , unsigned long long document)
{
	if (::insert(root , document , it , end))
		++inverted_size;
}

InvertedNode * InvertedIndex::operator[](Token const & token)
{
	return const_cast<InvertedNode *>(static_cast<InvertedIndex const &>(*this)[token]);
}

InvertedNode const * InvertedIndex::operator[](Token const & token) const
{
	return ::retrieve(root , token.cbegin() , token.cend());
}

InvertedNode * InvertedIndex::at(Token::const_iterator & it , Token::const_iterator const & end)
{
	return const_cast<InvertedNode *>(static_cast<InvertedIndex const &>(*this).at(it , end));
}

InvertedNode const * InvertedIndex::at(Token::const_iterator & it , Token::const_iterator const & end) const
{
	return ::retrieve(root , it , end);
}

size_t InvertedIndex::size() const
{
	return inverted_size;
}