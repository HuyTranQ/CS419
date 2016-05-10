#pragma once

#include "InvertedIndex.hpp"
#include "TokenContainer.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>

using wchar = wchar_t;

class Tokenizer
{
public:
	Tokenizer();
	unsigned merge(std::string const & path , InvertedIndex & inverted_index);
private:
	std::unordered_set<wchar> delimiter_;
	std::unordered_map<wchar , wchar> const character_;
	TokenContainer stopword_ , vocabulary;
};