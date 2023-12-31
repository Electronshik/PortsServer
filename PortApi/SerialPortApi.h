#pragma once
#include "main.h"

namespace SerialPortApi
{
	enum class ErrorCode
	{
		Ok,
		Error,
		PortNotExists,
		PortClosed,
	};

	extern std::map<ErrorCode, std::string> ErrorString;

	auto GetPortsList() -> std::vector<std::string>;
	auto OpenPort(std::string &port_name, SerialPortConfig &port_config) -> ErrorCode;
	auto ClosePort(std::string &port_name) -> ErrorCode;
	auto Send(std::string &port_name, std::string &cmd) -> ErrorCode;
	auto Receive(std::string &port_name) -> std::string;
	auto GetTestData() -> std::string;
}
