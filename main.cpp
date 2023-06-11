#include "httplib.h"
#include "Model.h"
#include "View.h"
#include <iostream>

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

	model.AddConfiguration("config");
	model.AddConfiguration("test");
	model.DeleteConfiguration("config");
	model.AddCommand("test", "cmd2test", "0xffff");
	model.DeleteCommand("test", "cmd2test");
	// auto result = model.GetConfigurations();
	// for(auto& el : result)
	// {
	// 	std::cout << el.c_str() << std::endl;
	// }

	server.listen("localhost", 80);
}
