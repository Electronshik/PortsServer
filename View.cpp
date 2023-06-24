#include "View.h"

#include <json.hpp>
#include <inja.hpp>

#include <fstream>
#include <iostream>

using namespace inja;

static Environment InjaEnv {"../html/"};

View::View()
{
}

View::~View()
{
}

std::string View::GetIndex(std::vector<std::string> &configurations, std::string &active_config, SerialPortConfig &port_config)
{
	// std::ifstream index("../html/index.html");
	// std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());

	json data;
	data["header"] = "Active configuration:";
	data["ports"] = {"COM1", "COM2"};
	data["configs"] = configurations;
	data["config_selected"] = active_config;

	data["all_port_speed"] = PortSpeed;
	data["all_port_databits"] = PortDatabits;
	data["all_port_parity"] = PortParity;
	data["all_port_stopbits"] = PortStopbits;
	data["all_port_flowcontrol"] = PortFlowcontrol;

	data["port_speed"] = port_config.Speed;
	data["port_databits"] = port_config.Databits;
	data["port_parity"] = port_config.Parity;
	data["port_stopbits"] = port_config.Stopbits;
	data["port_flowcontrol"] = port_config.Flowcontrol;

	// Environment env;

	return InjaEnv.render_file("index.html", data);
}
