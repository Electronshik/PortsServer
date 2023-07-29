#include "View.h"

#include <json.hpp>
#include <inja.hpp>

#include <fstream>
#include <iostream>

#include "SerialApi.h"

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
	data["ports"] = SerialApi::GetPortsList();	//{"COM1", "COM2"};
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

	std::string rendered_commands;
	json cmds;
	int cmd_iter = 1;
	for (Command &cmd : commands)
	{
		cmds["name"] = cmd.Name;
		cmds["cmd"] = cmd.Cmd;
		cmds["id"] = "cmd" + std::to_string(cmd_iter);
		// std::string cmd_template = "<p>{{ name }} : {{ cmd }}</p>";
		std::string cmd_template = "<div class=\"row\"><div class=\"col-sm-2\"><button type=\"button\" class=\"btn btn-success\"	\
			onclick=\"portSendCommand('{{ id }}');\">{{ name }}</button></div>	\
			<div class=\"col-sm-2\" id=\"{{ id }}\"><p>{{ cmd }}</p></div></div>";
		rendered_commands.append(InjaEnv.render(cmd_template, cmds));
		cmd_iter++;
	}

	// data["commands"] = {{ {"name", "name1"}, {"cmd", "cmd1"} }, { {"name", "name2"}, {"cmd", "cmd2"} }};
	data["commands"] = rendered_commands;

	return InjaEnv.render_file("index.html", data);
}
