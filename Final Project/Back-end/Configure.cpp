#include "Configure.hpp"
#include "Dictionary.hpp"

#include <boost\filesystem.hpp>
#include <boost/iostreams/device/mapped_file.hpp>
#include <chrono>
#include <iostream>
#include <fstream>
#include <locale>
#include <codecvt>

namespace fs = ::boost::filesystem;

using std::chrono::milliseconds;
using std::chrono::duration_cast;
using std::chrono::high_resolution_clock;
using fs::exists;
using fs::directory_iterator;
using fs::is_directory;
using fs::is_regular_file;
using std::cerr;
using std::cout;
using std::string;
using std::wofstream;

void insert_document(directory_iterator & it)
{
	auto endit = directory_iterator();
	while (it != endit)
	{
		if (is_regular_file(it->path()))
			Dictionary::open().learn_document(it);
		else if (is_directory(it->path()))
			insert_document(directory_iterator(it->path()));
		++it;
	}
}

void initialize(std::string const & corpus_path)
{
	if (!exists(corpus_path))
	{
		cerr << "Not a path: \"" << corpus_path << "\".\n";
		return;
	}
	if (!is_directory(corpus_path))
	{
		cerr << "Not a directory: \"" << corpus_path << "\".\n";
		return;
	}
	auto start = high_resolution_clock::now();
	insert_document(directory_iterator(corpus_path));
	auto finish = high_resolution_clock::now();
	cout << "Read documents: " << duration_cast<milliseconds>(finish - start).count() << "ms\n";
	start = high_resolution_clock::now();
	wofstream output("terms.txt" , wofstream::binary);
	output.imbue(std::locale(std::locale() , new std::codecvt_utf16<wchar_t , 0x10ffff , std::little_endian>()));
	Dictionary::open().print(output);
	output.close();
	finish = high_resolution_clock::now();
	cout << "Writing inverted index to file: " << duration_cast<milliseconds>(finish - start).count() << "ms\n";
	start = high_resolution_clock::now();
	Dictionary::open().construct_models(output);
	finish = high_resolution_clock::now();
	cout << "Construct models: " << duration_cast<milliseconds>(finish - start).count() << "ms\n";
}