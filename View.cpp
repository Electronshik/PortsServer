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

std::string replace_with_copy(std::string& str, const std::string& from, const std::string& to) {
	size_t start_pos = str.find(from);
	if(start_pos == std::string::npos)
		return "";

	std::string str_copy = str;
	str_copy.replace(start_pos, from.length(), to);
	return str_copy;
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

	size_t for_begin_pos = body.find("{{for:}}");
	if (for_begin_pos != std::string::npos)
	{
		size_t for_inner_begin_pos = for_begin_pos + strlen("{{for:}}");
		size_t for_end_pos = body.find("{{:for}}");
		if ((for_end_pos > for_begin_pos) && (for_end_pos != std::string::npos))
		{
			std::string inner_substr = body.substr(for_inner_begin_pos, for_end_pos - for_inner_begin_pos);
			std::cout << inner_substr << std::endl;

			std::string replace_to;
			unsigned int index = 1;
			for (auto& el : configurations)
			{
				replace_to.append(inner_substr);
				replace(replace_to, "{{index}}", std::to_string(index));
				replace(replace_to, "{{selected}}", index == 1 ? "selected" : "");
				replace(replace_to, "{{name}}", el);
				index++;
			}
			body.replace(for_begin_pos, for_end_pos + strlen("{{:for}}") - for_begin_pos, replace_to);
		}
	}

	return body;
}
