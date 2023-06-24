#pragma once
#include "main.h"

class View
{
	public:
		View();
		~View();
		std::string GetIndex(std::vector<std::string> &configurations, std::string &active_config, SerialPortConfig &port_config);
};
