#include "NamedPipeServer.hpp"
#include "Configure.hpp"
#include "Dictionary.hpp"
#include <iostream>

using std::string;
using std::to_string;

const int MAX = 1024;

NamedPipeServer::NamedPipeServer(wchar_t * name)
{
	pipe = CreateNamedPipe(name , PIPE_ACCESS_DUPLEX , PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT ,
						   PIPE_UNLIMITED_INSTANCES , MAX , MAX , 120000 , NULL);
	if (pipe == INVALID_HANDLE_VALUE)
		throw "Cannot open pipe";

	request = new wchar_t[MAX];
	response = new wchar_t[MAX];
}

NamedPipeServer::~NamedPipeServer()
{
	CloseHandle(pipe);
	delete[] request;
	delete[] response;
}

void NamedPipeServer::start()
{
	while (true)
	{
		BOOL success = FALSE;
		DWORD byte_read = 0 , byte_written = 0 , byte_write = 0;

		BOOL connected = ConnectNamedPipe(pipe , NULL) ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
		if (!connected)
		{
			CloseHandle(pipe);
			throw "Cannot connect pipe";
		}

		//	Read request from client;
		success = ReadFile(pipe , request , MAX * sizeof(wchar_t) , &byte_read , NULL);
		if (!success || byte_read == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
				break;
			else
				throw (string("Cannot read request from client. Error: ") + to_string(GetLastError())).c_str();
		}

		//	Handle Lots of things

		switch (request[0])
		{
			case L'a':
				initialize(request + 1);
				response[0] = L'c';
				byte_write = 2;
				break;
			case L'q':
			{
				RetrievalResult result = Dictionary::open().retrieve_documents(request + 1 , request + byte_read / 2);
				int counter = 0;
				for (auto it = result.ranked_retrieved_document_.crbegin() ,
					 endit = result.ranked_retrieved_document_.crend();
					 it != endit; ++it)
				{
					for (auto const & c : Dictionary::open().document_name(it->second))
					{
						response[counter] = c;
						++counter;
					}
					response[counter] = L'\n';
					++counter;
				}
				byte_write = 2 * counter;
			}
				break;
			default:
				break;
		}

		//	Respond to client
		success = WriteFile(pipe , response , byte_write , &byte_written , NULL);
		std::cout << byte_written << "bytes written\n";
		if (!success)
			throw "Response failed";
		byte_read = 0;
		byte_written = 0;
		FlushFileBuffers(pipe);
		DisconnectNamedPipe(pipe);
		std::cout << "done\n";
	}
}