#pragma once

#include <string>
#include <vector>

class View
{
	public:
		View();
		~View();
		std::string GetIndex(std::vector<std::string> configurations);
};
