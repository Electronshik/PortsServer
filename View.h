#pragma once
#include "main.h"

class View
{
	public:
		View();
		~View();
		auto GetIndex(std::vector<std::string> &configurations, std::string &active_config,
			SerialPortConfig &port_config, std::vector<Command> &commands) -> std::string;
};
