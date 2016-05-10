#pragma once

#include "InvertedIndex.hpp"
#include "TokenContainer.hpp"
#include "Document.hpp"
#include "Tokenizer.hpp"

#include <boost/filesystem.hpp>
#include <string>
#include <unordered_map>

using DocumentSet = std::unordered_map<unsigned , Document>;

class Dictionary
{
public:
	static Dictionary & open();

	Dictionary();
	void learn_document(boost::filesystem::directory_iterator const & file);
	void construct_models();
	void print(std::wofstream & output);
	
private:
	Tokenizer tokenizer;
	DocumentSet document_;
	InvertedIndex inverted_index;
	TokenContainer term_space;
};