#include "main.h"
#include "httplib.h"
#include "Model.h"
#include "View.h"
#include "Api.h"
#include "json.hpp"
#include <format>
#include <regex>
#include <memory>
#include <type_traits>

#include "SerialPort.h"

import Utils;

using namespace httplib;
using json = nlohmann::json;

std::string HtmlGlobalPath{"../../"};
int ServerPort = 80;

std::map<ErrorCode, std::string> ErrorString = {
	{ ErrorCode::Ok, "Ok" },
	{ ErrorCode::Error, "Error" }
};

SerialPortSettings<std::string, std::vector<std::string>> SerialPortAllSettings = {
	.Speed =		Setting({"9600", "19200", "115200"},	"115200"),
	.Databits =		Setting({"7", "8"},						"8"),
	.Parity =		Setting({"None", "Odd", "Even"},		"None"),
	.Stopbits =		Setting({"1", "2"},						"1"),
	.Flowcontrol =	Setting({"None", "Yes"},				"None"),
};

auto ParsePortConfig(const std::string &str, SerialPortConfig *config) -> bool
{
	std::smatch config_match;
	int param_num = 0;

	for (auto& [name, value] : config->ParamsList)
	{
		std::string pat{name};
		std::regex pattern((pat + "=([a-zA-Z0-9]+)").c_str());
		if (std::regex_search(str, config_match, pattern))
		{
			if (SerialPortAllSettings.ContainsValue(name, config_match[1].str()))
			{
				param_num++;
				value->assign(config_match[1]);
			}
		}
	}

	return param_num == config->ParamsList.size();
}

auto ParseGetPostParam(const std::string &str, const std::string &name) -> std::optional<std::string>
{
	std::smatch match;
	std::regex pattern(name + "=([a-zA-Z0-9]+)");
	if (std::regex_search(str, match, pattern))
	{
		std::string result = match[1];
		return result;
	}
	return std::nullopt;
}

void CreateDefaultConfiguration(Model &model)
{
	SerialPortConfig PortConfig;
	model.AddConfiguration("test", PortConfig);
	PortConfig.Speed = "9600";
	PortConfig.Stopbits = "2";
	model.AddCommand("test", "test_cmd", "0x01");
	model.AddCommand("test", "test_cmd2", "0x02");
}

auto main(int argc, char* argv[]) -> int
{
	static_assert(std::is_move_constructible_v<Command>);
	static_assert(std::is_nothrow_move_constructible_v<Command>);

	if (argc > 1)
	{
		print_nl("HtmlGlobalPath: {}", argv[1]);
		HtmlGlobalPath.assign(argv[1]);
	}

	if (argc > 2)
	{
		print_nl("ServerPort: {}", argv[2]);
		ServerPort = atoi(argv[2]);
	}

	Server server;
	Model model("Configurations.sqlite");
	View view;

	auto result = model.GetConfigurations();
	if (!result.empty())
	{
		for (auto& el : result)
			std::cout << "Found configurations: " << el.c_str() << std::endl;
	}
	else
	{
		CreateDefaultConfiguration(model);
	}

	server.set_logger([](const Request &req, const Response &res)
	{
		if (req.method == "GET")
		{
			std::string params{""};
			if (!req.params.empty())
				params = "?" + detail::params_to_query_str(req.params);

			print_nl("Log GET request: {}{}", req.path, params);
		}
		else if (req.method == "POST")
		{
			print_nl("Log POST request: {}", req.path);
			print_nl("\tWith body: {}", req.body);
		}
		else
		{
			print_nl("Log Other request: {}", req.path);
		}
	});

	server.Get("/", [&view, &model](const Request &req, Response &res)
	{
		std::string active_config = "";
		if (req.has_header("Cookie"))
		{
			std::string cookie = req.get_header_value("Cookie", 0);	//todo: parse first cookie
			if (auto cookie_config = ParseGetPostParam(cookie, "active"); cookie_config)
				active_config = cookie_config.value();
			print_nl("Active config (from cookie): ()", active_config.c_str());
		}

		ConfigList configurations = model.GetConfigurations();
		bool active_conf_found = false;
		if (active_config != "")
		{
			for (auto &config : configurations)
			{
				if (config == active_config)
				{
					active_conf_found = true;
					std::cout << "Active config found: " << active_config << std::endl;
				}
			}
		}

		if (!active_conf_found)
		{
			active_config = configurations.at(0);
			res.set_header("Set-Cookie", "active=" + active_config);
		}

		res.set_content(view.GetIndex(), "text/html");
	});

	server.Get(R"(/(html/[-/_\\.\d\w]+(\.css|\.js)))", [](const Request &req, Response &res)
	{
		std::string static_path = HtmlGlobalPath + req.matches[1].str();
		std::ifstream index(static_path.c_str());
		std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());
		std::string content_type = req.matches[2].str() == ".css" ? "text/css" : "text/javascript";
		res.set_content(body, content_type);
	});

	server.Get("/getconfiglist", [&model](const Request &req, Response &res)
	{
		json data;
		data["configurations"] = model.GetConfigurations();
		res.set_content(data.dump(), "application/json");
	});

	server.Get("/getallparams", [&model](const Request &req, Response &res)
	{
		json speed; 	   	speed["name"] = "speed";		speed["label"] = "Speed";
							speed["values"] = SerialPortAllSettings.Speed.GetAllValues();
		json databits; 	databits["name"] = "databits";	  databits["label"] = "Data bits";
						databits["values"] = SerialPortAllSettings.Databits.GetAllValues();
		json parity;		parity["name"] = "parity";		parity["label"] = "Parity";
							parity["values"] = SerialPortAllSettings.Parity.GetAllValues();
		json stopbits; 	stopbits["name"] = "stopbits";	  stopbits["label"] = "Stop Bits";
						stopbits["values"] = SerialPortAllSettings.Stopbits.GetAllValues();
		json flowctrl; 	flowctrl["name"] = "flowcontrol"; flowctrl["label"] = "Flow Ctrl";
						flowctrl["values"] = SerialPortAllSettings.Flowcontrol.GetAllValues();

		res.set_content(std::format("[\n{},\n{},\n{},\n{},\n{}\n]",
			speed.dump(), databits.dump(), parity.dump(), stopbits.dump(), flowctrl.dump()), "application/json");
	});

	server.Get("/getconfigparams", [&model](const Request &req, Response &res)
	{
		auto config = model.GetConfiguration(req.get_param_value("config").c_str());

		json speed;			   speed["name"] = "speed";			   speed["value"] = config.Speed;
		json databits; 		databits["name"] = "databits"; 		databits["value"] = config.Databits;
		json parity; 		  parity["name"] = "parity"; 		  parity["value"] = config.Parity;
		json stopbits; 		stopbits["name"] = "stopbits"; 		stopbits["value"] = config.Stopbits;
		json flowctrl; 		flowctrl["name"] = "flowcontrol"; 	flowctrl["value"] = config.Flowcontrol;

		res.set_content(std::format("[\n{},\n{},\n{},\n{},\n{}\n]",
			speed.dump(), databits.dump(), parity.dump(), stopbits.dump(), flowctrl.dump()), "application/json");
	});

	server.Post("/addnewconfig", [&model](const Request &req, Response &res)
	{
		auto result = ErrorCode::Error;
		if (auto config_name = ParseGetPostParam(req.body, "config_name"); config_name)
		{
			SerialPortConfig port_config;
			if (ParsePortConfig(req.body, &port_config))
			{
				model.AddConfiguration(config_name.value().c_str(), port_config);
				result = ErrorCode::Ok;
			}
		}
		json data;
		data["result"] = ErrorString[result];
		res.set_content(data.dump(), "application/json");
	});

	server.Get("/deleteconfig", [&model](const Request &req, Response &res)
	{
		auto result = ErrorCode::Error;

		model.DeleteConfiguration(req.get_param_value("config").c_str());
		json data;
		data["result"] = ErrorString[ErrorCode::Ok];
		res.set_content(data.dump(), "application/json");
	});

	server.Post("/saveconfigparams", [&model](const Request &req, Response &res)
	{
		auto result = ErrorCode::Error;
		if (auto config_name = ParseGetPostParam(req.body, "config_name"); config_name)
		{
			SerialPortConfig port_config;
			if (ParsePortConfig(req.body, &port_config))
			{
				model.UpdateConfiguration(config_name.value().c_str(), port_config);
			}

			if (auto config_new_name = ParseGetPostParam(req.body, "config_new_name"); config_new_name)
			{
				model.RenameConfiguration(config_name.value().c_str(), config_new_name.value().c_str());
			}
			result = ErrorCode::Ok;
		}
		json data;
		data["result"] = ErrorString[result];
		res.set_content(data.dump(), "application/json");
	});

	server.Get("/getconfigcommands", [&model](const Request &req, Response &res)
	{
		json data;

		auto result = ErrorCode::Error;
		std::string config_name = req.get_param_value("config");
		if (config_name != "")
		{
			auto commands = model.GetCommands(config_name.c_str());
			std::vector<json> outjson_vec;
			std::transform(commands.begin(), commands.end(), std::back_inserter(outjson_vec), [](Command& cmd) {
				json command_json;
				command_json["cmd"] = cmd.Cmd;
				command_json["name"] = cmd.Name;
				return command_json;
			});

			data["commands_array"] = outjson_vec;
			result = ErrorCode::Ok;
		}
		data["result"] = ErrorString[result];
		res.set_content(data.dump(), "application/json");
	});

	server.Get("/api/getportslist", Api::GetPortsList);
	server.Post("/api/openport", Api::OpenPort);
	server.Post("/api/closeport", Api::ClosePort);
	server.Post("/api/sendtoport", Api::SendToPort);
	server.Post("/api/readfromport", Api::ReadFromPort);
	server.Get("/api/gettestdata", Api::GetTestData);


	// server.Get("/body-header-param", [](const Request &req, Response &res)
	// {
	// 	if (req.has_header("Content-Length")) {
	// 		auto val = req.get_header_value("Content-Length");
	// 	}
	// 	if (req.has_param("key")) {
	// 		auto val = req.get_param_value("key");
	// 	}
	// 	res.set_content(req.body, "text/plain");
	// });

	// SerialPortConfig PortConfig;
	// model.AddConfiguration("config", PortConfig);
	// PortConfig.Speed = "9600";
	// PortConfig.Stopbits = "2";
	// model.AddConfiguration("test", PortConfig);
	// // model.DeleteConfiguration("config");
	// model.AddCommand("test", "cmdtest", "0xffff");
	// model.AddCommand("test", "cmd2test", "0xffff");
	// model.AddCommand("config", "config cmd", "0x11ff");
	// model.AddCommand("config", "config new cmd", "0x22ff");
	// model.DeleteCommand("test", "cmd2test");

/*
	SerialPortConfig port_config;
	std::string name = "\\\\.\\COM3";
	SerialPort Port(name, port_config);
	SerialPort TwoPort(name, port_config);

	char burr[] = "qwerty\r\n";
	Port.Write(burr, 8);

	char readBuff[64];
	memset(readBuff, 0, sizeof(readBuff));
	int len = Port.Read(readBuff);
	if(len)
		printf("First Com Received: %s \r\n", readBuff);
	else
		printf("\nReading terminated. Last error %d\n", GetLastError());

	for (size_t i = 0; i < 10; i++)
	{
		int len = Port.Read(readBuff);
		if(len)
			printf("Com Received: %s \r\n", readBuff);
	}

	Port.~Port();
*/
	std::vector<std::string> ports = SerialPort::GetPortsList();
	for(auto &port : ports)
	{
		std::cout << "Port exists: " << port << std::endl;
	}

	print_nl("Server listen at port: {}", ServerPort);
	server.listen("localhost", ServerPort);
}
