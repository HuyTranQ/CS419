#include "Tokenizer.hpp"

#include <iostream>
#include <deque>
#include <fstream>
#include <codecvt>
#include <locale>
#include <chrono>

using boost::iostreams::mapped_file;
using std::wstring;
using std::cerr;
using std::cout;
using std::vector;
using std::unordered_set;
using std::unordered_map;
using std::deque;
using std::wofstream;
using std::string;
using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;

size_t const WCHAR_SIZE = (1 << (sizeof(wchar) * 8));

unordered_map<wchar , wchar> initialize_character_set()
{	//	Initialize character set
	mapped_file reader("vietnamese-characterset.txt" , mapped_file::readonly);

	if (!reader.is_open())
		//	Cannot read character set => cannot proceed on
		throw "Cannot find \"vietnamese-characterset.txt\"\n";

	wchar const * it = reinterpret_cast<wchar const *>(reader.const_begin()) + 1;
	wchar const * endit = reinterpret_cast<wchar const *>(reader.const_end());

	unordered_map<wchar , wchar> result;
	while (it != endit)
	{	//	The first character is a lower case
		wchar a = *it;
		it += 2;
		//	The second character is an upper case
		wchar b = *it;
		it += 3;
		result[a] = a;	//	lower case is lowered to itself
		result[b] = a;	//	upper case is lowered
	}
	
	reader.close();

	return result;
}

unordered_set<wchar> initialize_delimiters(wchar const * delimiter_)
{
	unordered_set<wchar> result;
	while ((*delimiter_) != L'\0')
	{
		result.insert(*delimiter_);
		++delimiter_;
	}
	return result;
}

Tokenizer::Tokenizer()
	: character_(initialize_character_set())
	, delimiter_(initialize_delimiters(L".,;:?+#=@|_`-*/^$%~&!\\'\"()<>[]{}\0\r\n\t\b\a\v\f\r"))
{
	auto start = high_resolution_clock::now();
	
	char const * filename_[] = {
		"vietnamese.txt" ,
		"vietnamese-stopwords.txt"
	};
	
	TokenContainer * data_[2] = {
		&vocabulary , &stopword_
	};

	auto not_character = character_.end();

	for (int i = 0; i < 2; ++i)
	{		
		boost::iostreams::mapped_file reader(filename_[i] , mapped_file::readonly);
		
		if (!reader.is_open())
			//	Cannot read vocabulary, cannot proceed on
			throw "Cannot open " + string(filename_[i]) + " to read vocabulary.\n";
		
		wchar const * it , *endit;
		it = 1 + reinterpret_cast<wchar const *>(reader.const_begin());
		endit = reinterpret_cast<wchar const *>(reader.const_end());

		while (it != endit)
		{
			vector<wstring> token;
			while (true)
			{
				auto begin = it;
				while (character_.find(*it) != not_character)
					++it;
				token.push_back(wstring(begin , it - begin));
				if ((*it) == L'\n' || (*it) == L'\0')
					break;
				++it;
			}
			data_[i]->insert(token);
			++it;
		}
		reader.close();
	}
	auto finish = high_resolution_clock::now();
	cout << "Reading vocabulary and stop words: " << duration_cast<milliseconds>(finish - start).count() << "ms\n";
}

unsigned Tokenizer::merge(std::string const & path , InvertedIndex & inverted_index)
{
	//	Counter for document identifiers
	static unsigned document_id = 0;
	++document_id;

	boost::iostreams::mapped_file reader(path , mapped_file::readonly);
	if (!reader.is_open())
	{	//	Reader cannot open file
		cerr << "Cannot open \"" << path << "\" to execute lexcial analysis\n";
		return document_id;
	}


	wchar const * it = 1 + reinterpret_cast<wchar const *>(reader.const_begin());
	wchar const * endit = reinterpret_cast<wchar const *>(reader.const_end());
	
	if (it == nullptr || endit == nullptr || it == endit)
		return document_id;

	vector<wstring> token;

	auto not_character = character_.end();
	auto not_delimiter = delimiter_.end();

	while (it != endit)
	{	//	Read a sequence of 6 words
		size_t counter = 6 - token.size();
		bool separated = false;
		for (; counter > 0; --counter)
		{	//	Eliminate all delimiters
			while (it != endit && character_.find(*it) == not_character)
			{
				if (delimiter_.find(*it) != not_delimiter)
				{	//	If it is a delimiter, eliminate all of 'em
					do
					{
						++it;
					} while (it != endit && delimiter_.find(*it) != not_delimiter);
					//	If it is a delimiter, it must not read more words
					//	=> There is no delimiter can present in the middle of a token
					separated = true;
					break;
				}
				++it;
			}
			
			if (it == endit)
			{	//	End of file
				separated = true;
				break;
			}

			if (separated)
				if (!token.empty())
					//	Delimiters found => must not read more words
					break;
				else
				{	//	Delimiteres found => Token is still empty => continue
					separated = false;
					++counter;
					continue;
				}

			wchar const * begin = it;
			do
			{	//	Read characters of a word in the token
				++it;
			} while (it != endit && character_.find(*it) != not_character);

			wstring origin = wstring(begin , it - begin);
			size_t length = origin.length();
			//	Lowercase the word
			for (size_t i = 0; i < length; ++i)
				origin[i] = character_.at(origin[i]);
			token.push_back(origin);
		}

		do
		{
			size_t counter;
			while (true)
			{	//	Eliminate stop words
				counter = stopword_.max_match(token);
				if (counter == 0)
					break;
				else
				{
					token = Token(token.cbegin() + counter, token.cend());
					if (token.empty())
						break;
				}
			}
			if (token.empty())
				break;
			counter = vocabulary.max_match(token);
			if (counter > 0)
			{	//	If there is a token in vocabulary, insert it
				inverted_index.insert(token.cbegin() , token.cbegin() + counter , document_id);
				token = Token(token.cbegin() + counter , token.cend());
			}
			else
			{	//	Otherwise, only insert the first word
				inverted_index.insert(token.cbegin() , token.cbegin() + 1 , document_id);
				token = Token(token.cbegin() + 1 , token.cend());
			}
		} while (separated && !token.empty());
	}
	reader.close();
	return document_id;
}