#include "Configure.hpp"
#include "NamedPipeServer.hpp"

#include <iostream>
#include <string>

using std::string;
using std::getline;
using std::cout;
using std::cin;

int main()
{
	try
	{
		NamedPipeServer pipe(L"\\\\.\\pipe\\ir");
		pipe.start();
	}
	catch (char const * exception)
	{
		cout << exception << '\n';
	}
	_getch();
	return 0;
}