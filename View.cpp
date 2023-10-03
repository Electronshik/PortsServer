#include "View.h"

#include <json.hpp>
#include <inja.hpp>

#include <fstream>
#include <iostream>

import Utils;

using namespace inja;

// static Environment InjaEnv {"../../html/"};
static std::unique_ptr<Environment> InjaEnv;

View::View()
{
	InjaEnv = make_unique<Environment>(HtmlGlobalPath + "html/");
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

	std::vector<std::string> outjson_vec;
	std::transform(commands.begin(), commands.end(), std::back_inserter(outjson_vec), [](Command& cmd) {
		json command_json;
		command_json["cmd"] = cmd.Cmd;
		command_json["name"] = cmd.Name;
		return command_json.dump();
	});

	std::string outjson_str{"\n"};
	outjson_str.append(join_with_sep(outjson_vec.begin(), outjson_vec.end(), ",\n"));

	data["commands_array"] = outjson_str;

	return InjaEnv->render_file("index.html", data);
}
