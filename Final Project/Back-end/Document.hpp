#pragma once

#include <vector>
#include <string>

using Model = std::vector<float>;


class Document
{
public:
	Model model;
	std::string name;

	Document()
		: name("unknown") , model(1 << 13 , 0.0f)
	{}

	Document(std::string const & name)
		: name(name) , model(1 << 13 , 0.0f)
	{}
};