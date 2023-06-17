#include "httplib.h"
#include "Model.h"
#include "View.h"
#include <iostream>
#include <format>

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

		res.set_content(view.GetIndex(model.GetConfigurations()), "text/html");
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

	server.Get("/editconfig", [&model](const Request& req, Response& res)
	{
		res.set_content("editconfig requested!", "text/plain");
	});

	server.Get("/getconfiguration", [&model](const Request& req, Response& res)
	{
		std::cout << req.get_param_value("config") << std::endl;
		auto config = model.GetConfiguration(req.get_param_value("config").c_str());
		std::cout << config.Speed << std::endl;
		std::string response = std::format("{{	\
			\"Speed\":\"{}\",	\
			\"Databits\":\"{}\",	\
			\"Parity\":\"{}\",	\
			\"Stopbits\":\"{}\",	\
			\"Flowcontrol\":\"{}\"	\
			}}", config.Speed, config.Databits, config.Parity, config.Stopbits, config.Flowcontrol);
		std::cout << response << std::endl;
		res.set_content(response.c_str(), "text/javascript");
	});

	server.Get("/getcommands", [&model](const Request& req, Response& res)
	{
		auto result = model.GetCommands("config");
		for (auto &el : result)
        	std::cout << '[' << el.Name << "] = " << el.Cmd << "; ";
		res.set_content("getcommands requested!", "text/plain");
	});

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
	PortConfig.Speed = 9600;
	PortConfig.Stopbits = 2;
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
