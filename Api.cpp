#include "Api.h"
#include "SerialApi.h"

namespace Api
{
	void GetPortsList(const Request& req, Response& res)
	{
		std::vector<std::string> ports = SerialApi::GetPortsList();
		std::string content;
		for(auto &port : ports)
		{
			content.append(port);
			if(port != ports.back())
				content.append(",");
			std::cout << "Port exists: " << port << std::endl;
		}
		res.set_content(content, "text/plain");
		// res.set_content("C0M1,COM2", "text/plain");
	}

	void OpenPort(const Request& req, Response& res)
	{
		std::string port_name = "";
		std::string answer = ErrorString[ErrorCode::Error];

		if (ParseGetPostParam(req.body, "port", port_name) == ErrorCode::Ok)
		{
			std::cout << "Param Port Name: " << port_name << std::endl;
			SerialPortConfig port_config;
			if (ParsePortConfig(req.body, &port_config))
			{
				SerialApi::OpenPort(port_name, port_config);
				answer = ErrorString[ErrorCode::Ok];
			}
		}
		std::cout << "Post openport: " << req.body << std::endl;
		res.set_content(answer, "text/plain");
	}

	void ClosePort(const Request& req, Response& res)
	{
		std::string port_name = "";
		std::string answer = ErrorString[ErrorCode::Error];

		if (ParseGetPostParam(req.body, "port", port_name) == ErrorCode::Ok)
		{
			std::cout << "Param Port Name: " << port_name << std::endl;
			SerialApi::ClosePort(port_name);
			answer = ErrorString[ErrorCode::Ok];
		}
		std::cout << "Post closeport: " << req.body << std::endl;
		res.set_content(answer, "text/plain");
	}

	void SendToPort(const Request& req, Response& res)
	{
		std::string port_name = "";
		std::string answer = ErrorString[ErrorCode::Error];

		if (ParseGetPostParam(req.body, "port", port_name) == ErrorCode::Ok)
		{
			std::string cmd = req.get_param_value("cmd");
			ParseGetPostParam(req.body, "cmd", cmd);
			std::cout << "Param Port Name: " << port_name << ", Cmd: " << cmd << std::endl;
			SerialApi::Send(port_name, cmd);
			answer = ErrorString[ErrorCode::Ok];
		}

		std::cout << "/api/sendtoport:" << req.get_param_value("cmd") << std::endl;
		res.set_content(answer, "text/plain");
	}

	void ReadFromPort(const Request& req, Response& res)
	{
		std::string port_name = "";
		std::string received = "";

		if (ParseGetPostParam(req.body, "port", port_name) == ErrorCode::Ok)
		{
			received = SerialApi::Receive(port_name);
			// received = ErrorString[ErrorCode::Ok];
		}

		if (received != "")
			std::cout << "Read from port: " << port_name << ", Val: " << received << std::endl;

		std::cout << "/api/readfromport:" << received << std::endl;
		res.set_content(received, "text/plain");
	}
}
