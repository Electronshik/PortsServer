#include "View.h"

#include <fstream>
#include <iostream>

bool replace(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if(start_pos == std::string::npos)
		return false;

	str.replace(start_pos, from.length(), to);
	return true;
}

bool for_replace(std::string& str, std::vector<std::string> loop_elements)
{
	size_t for_begin_pos = str.find("{{for:}}");
	if (for_begin_pos == std::string::npos)
		return false;

	size_t for_inner_begin_pos = for_begin_pos + strlen("{{for:}}");
	size_t for_end_pos = str.find("{{:for}}");
	if (for_end_pos == std::string::npos)
		return false;

	std::string inner_substr = str.substr(for_inner_begin_pos, for_end_pos - for_inner_begin_pos);

	std::string replace_to;
	unsigned int index = 1;
	for (auto& el : loop_elements)
	{
		replace_to.append(inner_substr);
		replace(replace_to, "{{index}}", std::to_string(index));
		replace(replace_to, "{{selected}}", index == 1 ? "selected" : "");
		replace(replace_to, "{{name}}", el);
		index++;
	}

	str.replace(for_begin_pos, for_end_pos + strlen("{{:for}}") - for_begin_pos, replace_to);
	return true;
}

View::View()
{
}

View::~View()
{
}

std::string View::GetIndex(std::vector<std::string> configurations)
{
	std::ifstream index("../html/index.html");
	std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());
	replace(body, "{{header}}", "Active configuration:");
	for_replace(body, configurations);

	return body;
}
