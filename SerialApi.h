#pragma once
#include "main.h"

#include "SerialPort.h"

namespace SerialApi {

	enum class ErrorCode
	{
		Ok,
		Error,
		PortNotExists,
		PortClosed,
	};

	extern std::map<ErrorCode, std::string> ErrorString;

	ErrorCode OpenPort(std::string &port_name, SerialPortConfig &port_config);
	ErrorCode ClosePort(std::string &port_name);
	ErrorCode Send(std::string &port_name, std::string &cmd);
	std::string Receive(std::string &port_name);

}
