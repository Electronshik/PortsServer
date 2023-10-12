#include "PortApi.h"
#include "SerialPortApi.h"
#include "json.hpp"

import Utils;
using json = nlohmann::json;

namespace PortApi
{
	void GetPortsList(const Request& req, Response& res)
	{
		json data;
		data["ports"] = SerialPortApi::GetPortsList();
		res.set_content(data.dump(), "application/json");
	}

	void OpenPort(const Request& req, Response& res)
	{
		auto result = ErrorCode::Error;
		if (auto port_name = ParseGetPostParam(req.body, "port"); port_name)
		{
			SerialPortConfig port_config;
			if (ParsePortConfig(req.body, &port_config))
			{
				SerialPortApi::OpenPort(port_name.value(), port_config);
				result = ErrorCode::Ok;
			}
		}
		json data;
		data["result"] = ErrorString[result];
		res.set_content(data.dump(), "application/json");
	}

	void ClosePort(const Request& req, Response& res)
	{
		auto result = ErrorCode::Error;
		if (auto port_name = ParseGetPostParam(req.body, "port"); port_name)
		{
			SerialPortApi::ClosePort(port_name.value());
			result = ErrorCode::Ok;
		}
		json data;
		data["result"] = ErrorString[result];
		res.set_content(data.dump(), "application/json");
	}

	void SendToPort(const Request& req, Response& res)
	{
		auto result = ErrorCode::Error;
		if (auto port_name = ParseGetPostParam(req.body, "port"); port_name)
		{
			if (auto cmd = ParseGetPostParam(req.body, "cmd"); cmd)
			{
				SerialPortApi::Send(port_name.value(), cmd.value());
				result = ErrorCode::Ok;
			}
		}
		json data;
		data["result"] = ErrorString[result];
		res.set_content(data.dump(), "application/json");
	}

	void ReadFromPort(const Request& req, Response& res)
	{
		auto result = ErrorCode::Error;
		std::string received = "";
		if (auto port_name = ParseGetPostParam(req.body, "port"); port_name)
		{
			result = ErrorCode::Ok;
			received = SerialPortApi::Receive(port_name.value());
			if (received != "")
				std::cout << "Read from port: " << port_name.value() << ", Val: " << received << std::endl;
		}
		json data;
		data["result"] = ErrorString[result];
		data["received"] = std::move(received);
		res.set_content(data.dump(), "application/json");
	}

	void GetTestData(const Request& req, Response& res)
	{
		std::string test_data = SerialPortApi::GetTestData();
		res.set_content(test_data, "application/json");
	}
}
