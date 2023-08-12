#include "View.h"

#include <json.hpp>
#include <inja.hpp>

#include <fstream>
#include <iostream>

using namespace inja;

static Environment InjaEnv {"../../html/"};

View::View()
{
}

View::~View()
{
}

auto View::GetIndex(std::vector<std::string> &configurations, std::string &active_config,
	SerialPortConfig &port_config, std::vector<Command> &commands) -> std::string
{
	// std::ifstream index("../html/index.html");
	// std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());

	json data;
	data["header"] = "Active configuration:";
	data["port"] = "{{ port }}";
	data["configs"] = configurations;
	data["config_selected"] = active_config;

	data["error_code_ok"] = ErrorString[ErrorCode::Ok];
	data["error_code_error"] = ErrorString[ErrorCode::Error];

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

	data["cmd"] = "{{ cmd }}";
	data["name"] = "{{ name }}";

	std::string commands_array = "\n";
	for (auto it = commands.begin(); Command &cmd : commands)
	{
		json command_json;
		command_json["cmd"] = cmd.Cmd;
		command_json["name"] = cmd.Name;
		commands_array.append(command_json.dump());
		if(++it != commands.end())
			commands_array.append(",\n");
	}

	// std::cout << "commands array:" << commands_array << std::endl;
	data["commands_array"] = commands_array;

	return InjaEnv.render_file("index.html", data);
}
