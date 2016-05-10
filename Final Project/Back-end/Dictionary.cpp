#include "Dictionary.hpp"

#include <boost/iostreams/device/mapped_file.hpp>
#include <map>
#include <string>
#include <cmath>

using std::multimap;
using boost::iostreams::mapped_file;
using std::wstring;
using std::shared_ptr;
using std::unordered_map;
using std::vector;
using std::log10f;

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
			token.push_back(wstring(begin , it - begin));
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
	unsigned id = tokenizer.merge(file->path().string() , inverted_index);
	document_[id] = Document(file->path().filename().string());
}

void Dictionary::print(std::wofstream & output)
{
	for (auto it = inverted_index.iterator(); !it.end(); ++it)
	{
		output << it.to_string() << L'\n';
		PostingList & posting_ = it.posting_list();
		for (auto jt = posting_.cbegin() , endjt = posting_.cend(); jt != endjt; ++jt)
			output << L' ' << jt->document << L'-' << jt->frequency;
		output << L'\n';
	}
}

void Dictionary::construct_models()
{
	unordered_map<unsigned , float> norm_;
	norm_.reserve(document_.size());
	for (auto it = document_.cbegin() , endit = document_.cend(); it != endit; ++it)
		norm_[it->first] = 0.0f;

	for (auto it = term_space.iterator(); !it.end(); ++it)
	{
		InvertedNode * node = inverted_index[it.token()];
		if (node == nullptr)
			continue;
		PostingList const * posting_ = node->posting_list();
		float idf = log10f((float) (document_.size() + 1) / (float) (posting_->size() + 1));
		size_t counter = 0;
		for (auto jt = posting_->cbegin() , endjt = posting_->cend(); jt != endjt; ++jt)
		{
			float tf_idf = jt->frequency * idf;
			document_[jt->document].model[counter] = tf_idf;
			norm_[jt->document] = tf_idf * tf_idf;
		}
	}

	for (auto it = document_.begin() , endit = document_.end(); it != endit; ++it)
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