#pragma once

#include <string>
#include <memory>
#include <Windows.h>
#include <cstdio>
#include <conio.h>
#include <tchar.h>
#include <strsafe.h>

class NamedPipeServer
{
public:
	NamedPipeServer(wchar_t * name);
	~NamedPipeServer();
	void start();
private:
	std::string name;
	HANDLE pipe;
	wchar_t * request;
	wchar_t * response;
};