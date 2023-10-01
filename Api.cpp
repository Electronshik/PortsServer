#include "Api.h"
#include "SerialApi.h"

import utils;

namespace Api
{
	void GetPortsList(const Request& req, Response& res)
	{
		std::vector<std::string> ports = SerialApi::GetPortsList();
		res.set_content(join_with_separator(ports.begin(), ports.end(), ","), "text/plain");
		// res.set_content("C0M1,COM2", "text/plain");
	}

	void OpenPort(const Request& req, Response& res)
	{
		std::string answer = ErrorString[ErrorCode::Error];

		if (auto port_name = ParseGetPostParam(req.body, "port"); port_name)
		{
			std::cout << "Param Port Name: " << port_name.value() << std::endl;
			SerialPortConfig port_config;
			if (ParsePortConfig(req.body, &port_config))
			{
				SerialApi::OpenPort(port_name.value(), port_config);
				answer = ErrorString[ErrorCode::Ok];
			}
		}
		std::cout << "Post openport: " << req.body << std::endl;
		res.set_content(answer, "text/plain");
	}

	void ClosePort(const Request& req, Response& res)
	{
		std::string answer = ErrorString[ErrorCode::Error];

		if (auto port_name = ParseGetPostParam(req.body, "port"); port_name)
		{
			std::cout << "Param Port Name: " << port_name.value() << std::endl;
			SerialApi::ClosePort(port_name.value());
			answer = ErrorString[ErrorCode::Ok];
		}
		std::cout << "Post closeport: " << req.body << std::endl;
		res.set_content(answer, "text/plain");
	}

	void SendToPort(const Request& req, Response& res)
	{
		std::string answer = ErrorString[ErrorCode::Error];

		if (auto port_name = ParseGetPostParam(req.body, "port"); port_name)
		{
			std::string cmd = req.get_param_value("cmd");
			cmd = ParseGetPostParam(req.body, "cmd").value();
			std::cout << "Param Port Name: " << port_name.value() << ", Cmd: " << cmd << std::endl;
			SerialApi::Send(port_name.value(), cmd);
			answer = ErrorString[ErrorCode::Ok];
		}

		std::cout << "/api/sendtoport:" << req.get_param_value("cmd") << std::endl;
		res.set_content(answer, "text/plain");
	}

	void ReadFromPort(const Request& req, Response& res)
	{
		std::string received = "";

		if (auto port_name = ParseGetPostParam(req.body, "port"); port_name)
		{
			received = SerialApi::Receive(port_name.value());
			// received = ErrorString[ErrorCode::Ok];
			if (received != "")
				std::cout << "Read from port: " << port_name.value() << ", Val: " << received << std::endl;
		}

		std::cout << "/api/readfromport:" << received << std::endl;
		res.set_content(received, "text/plain");
	}
}
