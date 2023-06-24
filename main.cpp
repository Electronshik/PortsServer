#include "main.h"
#include "httplib.h"
#include "Model.h"
#include "View.h"
#include <iostream>
#include <format>
#include <regex>

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

int main(void)
{
	using namespace httplib;

	Server server;
	Model model("Configurations.sqlite");
	View view;

	server.Get("/", [&view, &model](const Request& req, Response& res)
	{
		// std::ifstream index("../html/index.html");
		// std::string body((std::istreambuf_iterator<char>(index)), std::istreambuf_iterator<char>());
		// std::cout << req.path << " Ans len:" << body.length() << std::endl;

		std::string active_config = "";
		if (req.has_header("Cookie"))
		{
			std::string cookie = req.get_header_value("Cookie", 0);	//todo: parse first cookie

			const std::regex act_conf_regex("active=([a-z]+)");

			std::smatch cookie_match;
			if (std::regex_search(cookie, cookie_match, act_conf_regex))
			{
				active_config = cookie_match[1];
			}

			std::cout << "Active config (from cookie): " << active_config << std::endl;
		}

		// res.set_header("Set-Cookie", "active=test");
		// res.set_header("Set-Cookie", "last=first");

		ConfigList configurations = model.GetConfigurations();
		bool active_conf_finded = false;
		if (active_config != "")
		{
			for ( auto &config : configurations)
			{
				if (config == active_config)
				{
					active_conf_finded = true;
					std::cout << "Active config finded: " << active_config << std::endl;
				}
			}
		}

		if (!active_conf_finded)
			active_config = configurations.at(0);

		auto config = model.GetConfiguration(active_config.c_str());
		res.set_content(view.GetIndex(configurations, active_config, config), "text/html");
	});

	server.Get(R"(/(html/[-/_\\.\d\w]+(\.css|\.js)))", [](const Request& req, Response& res)
	{
		std::string static_path = "../" + req.matches[1].str();
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
		std::smatch config_match;
		unsigned int param_num = 0;
		SerialPortConfig port_config;
		std::string active_config = "";

		std::regex pattern("active_config=([a-zA-Z0-9]+)");
		if (std::regex_search(req.body, config_match, pattern))
		{
			param_num++;
			active_config = config_match[1];
			std::cout << "active_config: " << active_config << std::endl;
		}

		pattern = "config_speed=([a-zA-Z0-9]+)";
		if (std::regex_search(req.body, config_match, pattern))
		{
			param_num++;
			port_config.Speed = config_match[1];
			std::cout << "port_config.Speed: " << port_config.Speed << std::endl;
		}

		pattern = "config_databits=([a-zA-Z0-9]+)";
		if (std::regex_search(req.body, config_match, pattern))
		{
			param_num++;
			port_config.Databits = config_match[1];
			std::cout << "port_config.Databits: " << port_config.Databits << std::endl;
		}

		pattern = "config_parity=([a-zA-Z0-9]+)";
		if (std::regex_search(req.body, config_match, pattern))
		{
			param_num++;
			port_config.Parity = config_match[1];
			std::cout << "port_config.Parity: " << port_config.Parity << std::endl;
		}

		pattern = "config_stopbits=([a-zA-Z0-9]+)";
		if (std::regex_search(req.body, config_match, pattern))
		{
			param_num++;
			port_config.Stopbits = config_match[1];
			std::cout << "port_config.Stopbits: " << port_config.Stopbits << std::endl;
		}

		pattern = "config_flowcontrol=([a-zA-Z0-9]+)";
		if (std::regex_search(req.body, config_match, pattern))
		{
			param_num++;
			port_config.Flowcontrol = config_match[1];
			std::cout << "port_config.Flowcontrol: " << port_config.Flowcontrol << std::endl;
		}

		if ((!active_config.empty()) && (param_num == 6))
		{
			model.UpdateConfiguration(active_config.c_str(), port_config);
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

	// server.Get("/getcommands", [&model](const Request& req, Response& res)
	// {
	// 	auto result = model.GetCommands("config");
	// 	for (auto &el : result)
    //     	std::cout << '[' << el.Name << "] = " << el.Cmd << "; ";
	// 	res.set_content("getcommands requested!", "text/plain");
	// });

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

	SerialPortConfig PortConfig;
	model.AddConfiguration("config", PortConfig);
	PortConfig.Speed = "9600";
	PortConfig.Stopbits = "2";
	model.AddConfiguration("test", PortConfig);
	// model.DeleteConfiguration("config");
	model.AddCommand("test", "cmdtest", "0xffff");
	model.AddCommand("test", "cmd2test", "0xffff");
	model.AddCommand("config", "config cmd", "0x11ff");
	model.AddCommand("config", "config new cmd", "0x22ff");
	// model.DeleteCommand("test", "cmd2test");
	auto result = model.GetConfigurations();
	for(auto& el : result)
	{
		std::cout << el.c_str() << std::endl;
	}

	server.listen("localhost", 80);
}
