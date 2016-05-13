#include "Dictionary.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <map>
#include <string>
#include <cmath>
#include <chrono>

using std::multimap;
using boost::iostreams::mapped_file;
using std::wstring;
using std::string;
using std::shared_ptr;
using std::unordered_map;
using std::vector;
using std::log10f;
using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using std::ostream;

Dictionary::Dictionary()
{
	document_.reserve(45000);
	mapped_file reader("vector-space-models.txt");
	if (!reader.is_open())
		throw "Cannot read: \"vector-space-models.txt\".\n";
	auto it = 1 + reinterpret_cast<wchar const *>(reader.const_begin());
	auto endit = reinterpret_cast<wchar const *>(reader.const_end());
	while (it != endit)
	{
		Token token;
		while (true)
		{
			auto begin = it;
			while ((*it) != L' ' && (*it) != L'\n')
				++it;
			token.push_back(wstring(begin, it - begin));
			if ((*it) == L'\n')
				break;
			++it;
		}
		term_space.insert(token);
		++it;
	}
	reader.close();
}

Dictionary & Dictionary::open()
{
	static Dictionary dictionary;
	return dictionary;
}

void Dictionary::learn_document(boost::filesystem::directory_iterator const & file)
{
	unsigned id = tokenizer.merge(file->path().string(), inverted_index);
	document_[id] = Document(file->path().filename().string());
}

void Dictionary::print(std::wofstream & output)
{
	for (auto it = inverted_index.iterator(); !it.end(); ++it)
	{
		output << it.to_string() << L'\n';
		PostingList & posting_ = it.posting_list();
		for (auto jt = posting_.cbegin(), endjt = posting_.cend(); jt != endjt; ++jt)
			output << L' ' << jt->document << L'-' << jt->frequency;
		output << L'\n';
	}
}

void Dictionary::construct_models()
{
	unordered_map<unsigned, float> norm_;
	norm_.reserve(document_.size());
	for (auto it = document_.cbegin(), endit = document_.cend(); it != endit; ++it)
		norm_[it->first] = 0.0f;

	size_t counter = 0;
	for (auto it = term_space.iterator(); !it.end(); ++it, ++counter)
	{
		InvertedNode * node = inverted_index[it.token()];
		if (node == nullptr)
			continue;
		PostingList const * posting_ = node->posting_list();
		float idf = log10f((float)(document_.size() + 1) / (float)(posting_->size() + 1));
		for (auto jt = posting_->cbegin(), endjt = posting_->cend(); jt != endjt; ++jt)
		{
			float tf_idf = jt->frequency * idf;
			document_[jt->document].model[counter] = tf_idf;
			norm_[jt->document] += tf_idf * tf_idf;
		}
	}

	for (auto it = document_.begin(), endit = document_.end(); it != endit; ++it)
	{
		vector<float> & model = it->second.model;
		if (norm_.at(it->first) > 0.0f)
		{
			float norm = sqrtf(norm_.at(it->first));
			for (int i = 0; i < (1 << 13); ++i)
				model[i] /= norm;
		}
	}
}

RetrievalResult Dictionary::retrieve_documents(string const & path)
{
	RetrievalResult result;

	auto start = high_resolution_clock::now();

	// parse the query into tokens
	InvertedIndex query_ii;
	tokenizer.merge(path, query_ii);
	// build the query vector
	vector<float> query_vector((1 << 13), 0.0f);
	float norm = 0.0f;

	size_t counter = 0;
	for (auto it = term_space.iterator(); !it.end(); ++it, ++counter)
	{
		if (query_ii[it.token()] == nullptr)
			continue;

		InvertedNode * node = inverted_index[it.token()];
		if (node == nullptr)
			continue;
		PostingList const * posting_ = node->posting_list();
		float idf = log10f((float)(document_.size() + 1) / (float)(posting_->size() + 1));
		for (auto jt = posting_->cbegin(), endjt = posting_->cend(); jt != endjt; ++jt)
		{
			float tf_idf = jt->frequency * idf;
			query_vector[counter] = tf_idf;
			norm += tf_idf * tf_idf;
			result.relevant_document_.insert(jt->document);
		}
	}

	if (norm > 0.0f)
	{
		norm = sqrtf(norm);
		for (int i = 0; i < (1 << 13); ++i)
			query_vector[i] /= norm;
	}

	// Calculate the cosine similarity between the query vector and each of the document vector
	for (auto it = document_.cbegin(), endit = document_.cend(); it != endit; ++it)
	{
		vector<float> model = it->second.model;
		float dot_product = 0;
		float l1 = 0.0f, l2 = 0.0f;

		// calculate the dot product
		for (unsigned int i = 0; i < (1 << 13); ++i)
			dot_product += (query_vector[i] * model[i]);

		// since the lengths of both vectors are either 0 or 1 due to normalization
		// if either one of the 2 vectors has length 0, then the dot product is 0
		// otherwise we divide the dot product by the product of vector lengths, which is 1
		// therefore the cosine similarity is exactly the dot product
		if (dot_product != 0)
		{
			result.ranked_retrieved_document_.insert({ dot_product, it->first });
			result.unranked_retrieved_document_.push_back(it->first);
		}
	}

	auto finish = high_resolution_clock::now();
	result.retrieval_time = duration_cast<milliseconds>(finish - start).count();

	return result;
}

string const & Dictionary::document_name(unsigned const & id)
{
	return document_[id].name;
}

void RetrievalResult::print(ostream & output)
{
	output << ranked_retrieved_document_.size() << " results in " << retrieval_time << "ms\n";
	unsigned counter = 1;
	for (auto it = ranked_retrieved_document_.crbegin(), endit = ranked_retrieved_document_.crend(); it != endit; ++it, ++counter)
	{
		output << counter << ".\t\t\t" << Dictionary::open().document_name(it->second) << "\t" << it->first << "\n";
	}
}

EvaluationResult Dictionary::evaluate_result(const RetrievalResult& rr)
{
	EvaluationResult er;
	int n_retrieved_relevant = 0;
	auto it = rr.ranked_retrieved_document_.begin();

	for (int i = 1; i <= rr.ranked_retrieved_document_.size(); ++i, ++it)
	{
		float prec = 0;
		float recall = 0;
		
		/* Calculate number of retrieved and relevant docs */
		if (rr.relevant_document_.count(it->second) != 0)
			++n_retrieved_relevant;

		/* Calculate precision and recall for each level */
		prec = (float)n_retrieved_relevant / (float)i;
		recall = (float)n_retrieved_relevant / (float)rr.relevant_document_.size();

		er.prec_recall_.push_back(std::make_pair(prec, recall));
	}
	if (er.prec_recall_.size() != 0)
	{
		/* Calculate f-measure */
		int size = er.prec_recall_.size();
		float prec = er.prec_recall_[size - 1].first;
		float recall = er.prec_recall_[size - 1].second;
		er.f_measure = 2 * prec*recall / (prec + recall);
	}
}

std::vector<std::pair<float, float>> EvaluationResult::get_prec_recall()
{
	return prec_recall_;
}

float EvaluationResult::get_f_measure()
{
	return f_measure;
}