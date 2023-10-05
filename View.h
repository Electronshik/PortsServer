#pragma once
#include "main.h"

class View
{
	public:
		View();
		~View();
		auto GetIndex(std::vector<Command> &commands) -> std::string;
};
