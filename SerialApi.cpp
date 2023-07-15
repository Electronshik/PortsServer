#include "SerialApi.h"
#include "SerialPort.h"
#include <memory>

namespace SerialApi {

	std::vector<std::unique_ptr<SerialPort>> OpenedPorts;
	std::map<ErrorCode, std::string> ErrorString = {
		{ ErrorCode::Ok, "Ok" },
		{ ErrorCode::Error, "Error" },
		{ ErrorCode::PortNotExists, "PortNotExists" },
		{ ErrorCode::PortClosed, "PortClosed" },
	};

	ErrorCode OpenPort(std::string &port_name, SerialPortConfig &port_config)
	{
		OpenedPorts.push_back(std::make_unique<SerialPort>(port_name, port_config));
		return ErrorCode::Ok;
	}

	ErrorCode ClosePort(std::string &port_name)
	{
		auto it = std::find_if(OpenedPorts.begin(), OpenedPorts.end(), [&](std::unique_ptr<SerialPort>& ptr){ return ptr->GetName() == port_name; });
		if (it != OpenedPorts.end())
		{
			OpenedPorts.erase(it);
			std::cout << "Port removed: " << port_name << std::endl;
		}
		return ErrorCode::Ok;
	}

	ErrorCode Send(std::string &port_name, std::string &cmd)
	{
		if (OpenedPorts.empty())
			return ErrorCode::PortClosed;

		auto it = std::find_if(OpenedPorts.begin(), OpenedPorts.end(), [&](std::unique_ptr<SerialPort>& ptr){ return ptr->GetName() == port_name; });
		if (it == OpenedPorts.end())
			return ErrorCode::PortClosed;

		(*it)->Write((char*)cmd.c_str(), cmd.length());
		// (OpenedPorts.at(0).get())->Write((char*)cmd.c_str(), cmd.length());
		std::cout << "Send to port: " << cmd << std::endl;
		return ErrorCode::Ok;
	}

	std::string Receive(std::string &port_name)
	{
		std::string received = "";

		if (OpenedPorts.empty())
			return "";

		auto it = std::find_if(OpenedPorts.begin(), OpenedPorts.end(), [&](std::unique_ptr<SerialPort>& ptr){ return ptr->GetName() == port_name; });
		if (it == OpenedPorts.end())
			return "";

		char buff[64];
		int len = (*it)->Read(buff);
		for (int i = 0; i < len; i++)
			received += buff[i];

		// if (received != "")
		// 	std::cout << "Read from port: " << (OpenedPorts.at(0).get())->GetName() << ", Val: " << received << std::endl;

		return received;
	}

}
