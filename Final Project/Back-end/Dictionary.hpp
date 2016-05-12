#pragma once

#include "InvertedIndex.hpp"
#include "TokenContainer.hpp"
#include "Document.hpp"
#include "Tokenizer.hpp"

#include <boost/filesystem.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <map>
#include <iostream>

using DocumentSet = std::unordered_map<unsigned, Document>;

class RetrievalResult;

class Dictionary
{
public:
	static Dictionary & open();

	Dictionary();
	void learn_document(boost::filesystem::directory_iterator const & file);
	void construct_models();
	void print(std::wofstream & output);
	RetrievalResult retrieve_documents(std::string const & path);
	EvaluationResult evaluate_result(const RetrievalResult& rr);
	std::string const & document_name(unsigned const & id);

private:
	Tokenizer tokenizer;
	DocumentSet document_;
	InvertedIndex inverted_index;
	TokenContainer term_space;
};

/*
* Single-query retrieval result
*/
class RetrievalResult
{
public:
	friend class Dictionary;
	void print(std::ostream & output);

private:
	std::multimap<float, unsigned> ranked_retrieved_document_;
	std::vector<unsigned> unranked_retrieved_document_;
	std::unordered_set<unsigned> relevant_document_;
	long long retrieval_time;
};

class EvaluationResult
{
	friend class Dictionary;
public:
	std::vector<std::pair<float, float>> get_prec_recall();
	float get_f_measure();
private:
	std::vector<std::pair<float, float>> prec_recall_;
	float f_measure;
};