#include "Configure.hpp"

#include <iostream>
#include <string>

using std::string;
using std::getline;
using std::cout;
using std::cin;

int main()
{
	string path;
	cout << "Corpus: ";
	getline(cin , path);
	initialize(path);
	return 0;
}