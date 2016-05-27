#pragma once

#include <vector>
#include <string>

using Model = std::vector<float>;


class Document
{
public:
	Model model;
	std::wstring name;

	Document()
		: name(L"unknown") , model(1 << 13 , 0.0f)
	{}

	Document(std::wstring const & name)
		: name(name) , model(1 << 13 , 0.0f)
	{}
};