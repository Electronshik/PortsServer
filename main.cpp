#include "main.h"
#include "httplib.h"
#include "Model.h"
#include "View.h"
#include "Api.h"
#include <format>
#include <regex>
#include <memory>

#include "SerialPort.h"

using namespace httplib;

std::map<ErrorCode, std::string> ErrorString = {
	{ ErrorCode::Ok, "Ok" },
	{ ErrorCode::Error, "Error" }
};

std::array<std::string, 3> PortSpeed = 
{
	"9600",
	"19200",
	"115200"
};

std::array<std::string, 2> PortDatabits =
{
	"7",
	"8"
};

std::array<std::string, 3> PortParity =
{
	"None",
	"Odd",
	"Even"
};

std::array<std::string, 2> PortStopbits =
{
	"1",
	"2"
};

std::array<std::string, 2> PortFlowcontrol =
{
	"None",
	"Yes"
};

auto ParsePortConfig(const std::string &str, SerialPortConfig *config) -> bool
{
	std::smatch config_match;
	int param_num = 0;

	std::regex pattern("config_speed=([a-zA-Z0-9]+)");
	if (std::regex_search(str, config_match, pattern))
	{
		param_num++;
		config->Speed = config_match[1];
	}

	pattern = "config_databits=([a-zA-Z0-9]+)";
	if (std::regex_search(str, config_match, pattern))
	{
		param_num++;
		config->Databits = config_match[1];
	}

	pattern = "config_parity=([a-zA-Z0-9]+)";
	if (std::regex_search(str, config_match, pattern))
	{
		param_num++;
		config->Parity = config_match[1];
	}

	pattern = "config_stopbits=([a-zA-Z0-9]+)";
	if (std::regex_search(str, config_match, pattern))
	{
		param_num++;
		config->Stopbits = config_match[1];
	}

	pattern = "config_flowcontrol=([a-zA-Z0-9]+)";
	if (std::regex_search(str, config_match, pattern))
	{
		param_num++;
		config->Flowcontrol = config_match[1];
	}

	// std::cout << "Param Speed: " << config->Speed << std::endl;
	// std::cout << "Param Databits: " << config->Databits << std::endl;
	// std::cout << "Param Parity: " << config->Parity << std::endl;
	// std::cout << "Param Stopbits: " << config->Stopbits << std::endl;
	// std::cout << "Param Flowcontrol: " << config->Flowcontrol << std::endl;

	return param_num == 5;
}

auto ParseGetPostParam(const std::string &str, const std::string &name, std::string &result) -> ErrorCode
{
	std::smatch match;
	std::regex pattern(name + "=([a-zA-Z0-9]+)");
	if (std::regex_search(str, match, pattern))
	{
		result = match[1];
		return ErrorCode::Ok;
	}
	return ErrorCode::Error;
}

auto main(void) -> int
{
	Server server;
	Model model("Configurations.sqlite");
	View view;

	server.Get("/", [&view, &model](const Request& req, Response& res)
	{
		std::string active_config = "";
		if (req.has_header("Cookie"))
		{
			std::string cookie = req.get_header_value("Cookie", 0);	//todo: parse first cookie
			ParseGetPostParam(cookie, "active", active_config);
			std::cout << "Active config (from cookie): " << active_config << std::endl;
		}

		// res.set_header("Set-Cookie", "active=test");
		// res.set_header("Set-Cookie", "last=first");

		ConfigList configurations = model.GetConfigurations();
		bool active_conf_found = false;
		if (active_config != "")
		{
			for ( auto &config : configurations)
			{
				if (config == active_config)
				{
					active_conf_found = true;
					std::cout << "Active config finded: " << active_config << std::endl;
				}
			}
		}

		if (!active_conf_found)
		{
			active_config = configurations.at(0);
			res.set_header("Set-Cookie", "active=" + active_config);
		}

		auto config = model.GetConfiguration(active_config.c_str());
		auto commands = model.GetCommands(active_config.c_str());
		res.set_content(view.GetIndex(configurations, active_config, config, commands), "text/html");
	});

	server.Get(R"(/(html/[-/_\\.\d\w]+(\.css|\.js)))", [](const Request& req, Response& res)
	{
		std::string static_path = "../../" + req.matches[1].str();
		std::ifstream index(static_path.c_str());
		std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());
		std::cout << "Static path: " << static_path << ", Ext: " << req.matches[2].str() << std::endl;
		std::string content_type = req.matches[2].str() == ".css" ? "text/css" : "text/javascript";
		res.set_content(body, content_type);
	});

	server.Get("/deleteconfig", [&model](const Request& req, Response& res)
	{
		std::cout << "deleteconfig:" << req.get_param_value("config") << std::endl;
		model.DeleteConfiguration(req.get_param_value("config").c_str());
		res.set_content("deleteconfig success", "text/plain");
	});

	server.Post("/saveconfigparams", [&model](const Request& req, Response& res)
	{
		SerialPortConfig port_config;
		std::string active_config = "";

		ParseGetPostParam(req.body, "active_config", active_config);
		std::cout << "active_config: " << active_config << std::endl;

		if ((!active_config.empty()) && ParsePortConfig(req.body, &port_config))
		{
			std::string new_name = "";
			if (ParseGetPostParam(req.body, "config_new_name", new_name) == ErrorCode::Ok)
			{
				std::string new_config_process_flag = "";
				if (ParseGetPostParam(req.body, "new_config_process", new_config_process_flag) == ErrorCode::Ok)
				{
					model.AddConfiguration(new_name.c_str(), port_config);
					std::cout << "New config added: " << new_name << std::endl;
				}
				else
				{
					model.RenameConfiguration(active_config.c_str(), new_name.c_str());
					std::cout << "New name for config: " << new_name << std::endl;
				}
			}
			else
			{
				model.UpdateConfiguration(active_config.c_str(), port_config);
			}
			res.set_content("saveconfigparams successfully updated!", "text/plain");
		}
		else
		{
			res.set_content("saveconfigparams update error!", "text/plain");
		}
	});

	// server.Get("/getconfiguration", [&model](const Request& req, Response& res)
	// {
	// 	std::cout << req.get_param_value("config") << std::endl;
	// 	auto config = model.GetConfiguration(req.get_param_value("config").c_str());
	// 	std::cout << config.Speed << std::endl;
	// 	std::string response = std::format("{{	\
	// 		\"Speed\":\"{}\",	\
	// 		\"Databits\":\"{}\",	\
	// 		\"Parity\":\"{}\",	\
	// 		\"Stopbits\":\"{}\",	\
	// 		\"Flowcontrol\":\"{}\"	\
	// 		}}", config.Speed, config.Databits, config.Parity, config.Stopbits, config.Flowcontrol);
	// 	std::cout << response << std::endl;
	// 	res.set_content(response.c_str(), "text/javascript");
	// });

	server.Get("/api/getportslist", Api::GetPortsList);
	server.Post("/api/openport", Api::OpenPort);
	server.Post("/api/closeport", Api::ClosePort);
	server.Post("/api/sendtoport", Api::SendToPort);
	server.Post("/api/readfromport", Api::ReadFromPort);

	server.Get("/body-header-param", [](const Request& req, Response& res)
	{
		if (req.has_header("Content-Length")) {
			auto val = req.get_header_value("Content-Length");
		}
		if (req.has_param("key")) {
			auto val = req.get_param_value("key");
		}
		res.set_content(req.body, "text/plain");
	});

	server.Get("/stop", [&](const Request& req, Response& res)
	{
		server.stop();
	});

	// SerialPortConfig PortConfig;
	// model.AddConfiguration("config", PortConfig);
	// PortConfig.Speed = "9600";
	// PortConfig.Stopbits = "2";
	// model.AddConfiguration("test", PortConfig);
	// model.DeleteConfiguration("config");
	// model.AddCommand("test", "cmdtest", "0xffff");
	// model.AddCommand("test", "cmd2test", "0xffff");
	// model.AddCommand("config", "config cmd", "0x11ff");
	// model.AddCommand("config", "config new cmd", "0x22ff");
	// model.DeleteCommand("test", "cmd2test");
	auto result = model.GetConfigurations();
	for(auto& el : result)
	{
		std::cout << "Finded configurations: " << el.c_str() << std::endl;
	}

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

	printf("server.listen \r\n");
	server.listen("localhost", 80);
}
