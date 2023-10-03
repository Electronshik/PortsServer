#include "SerialApi.h"
#include "SerialPort.h"
#include "TestPort.h"
#include <memory>
#include <concepts>
#include <format>

import utils;

namespace SerialApi
{
	template<class T> concept PortTypeInterface =
	requires (T t)
	{
		{ T::GetPortsList() } -> std::same_as<std::vector<std::string>>;
		T(std::declval<std::string&>(), std::declval<SerialPortConfig&>());
		std::is_constructible<T>;
		std::is_destructible<T>;
		{ t.GetName() } -> std::same_as<std::string>;
		{ t.Write(std::declval<char*>(), int{}) } -> std::same_as<void>;
		{ t.Read(std::declval<char*>()) } -> std::same_as<int>;
	};

	using PortType = TestPort;
	static_assert(PortTypeInterface<PortType>);

	std::vector<std::unique_ptr<PortType>> OpenedPorts;
	std::map<ErrorCode, std::string> ErrorString = {
		{ ErrorCode::Ok, "Ok" },
		{ ErrorCode::Error, "Error" },
		{ ErrorCode::PortNotExists, "PortNotExists" },
		{ ErrorCode::PortClosed, "PortClosed" },
	};

	auto GetPortsList() -> std::vector<std::string>
	{
		return PortType::GetPortsList();
	}

	auto OpenPort(std::string &port_name, SerialPortConfig &port_config) -> ErrorCode
	{
		OpenedPorts.push_back(std::make_unique<PortType>(port_name, port_config));
		return ErrorCode::Ok;
	}

	auto ClosePort(std::string &port_name) -> ErrorCode
	{
		auto it = std::find_if(OpenedPorts.begin(), OpenedPorts.end(),
			[&](std::unique_ptr<PortType>& ptr){ return ptr->GetName() == port_name; });

		if (it != OpenedPorts.end())
		{
			OpenedPorts.erase(it);
			std::cout << "Port removed: " << port_name << std::endl;
		}
		return ErrorCode::Ok;
	}

	auto Send(std::string &port_name, std::string &cmd) -> ErrorCode
	{
		if (OpenedPorts.empty())
			return ErrorCode::PortClosed;

		auto it = std::find_if(OpenedPorts.begin(), OpenedPorts.end(),
			[&](std::unique_ptr<PortType>& ptr){ return ptr->GetName() == port_name; });

		if (it == OpenedPorts.end())
			return ErrorCode::PortClosed;

		(*it)->Write((char*)cmd.c_str(), cmd.length());
		// (OpenedPorts.at(0).get())->Write((char*)cmd.c_str(), cmd.length());
		std::cout << "Send to port: " << cmd << std::endl;
		return ErrorCode::Ok;
	}

	auto Receive(std::string &port_name) -> std::string
	{
		std::string received = "";

		if (OpenedPorts.empty())
			return "";

		auto it = std::find_if(OpenedPorts.begin(), OpenedPorts.end(), [&](std::unique_ptr<PortType>& ptr){ return ptr->GetName() == port_name; });
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

	auto GetTestData() -> std::string
	{
		std::vector<std::string> ports = PortType::GetPortsList();
		std::string test_data = std::format("{{	\
			\"ports\":\"[{}]\",	\
				\"errors\":{{	\
					\"Ok\":\"{}\",	\
					\"Ok\":\"{}\",	\
					\"Ok\":\"{}\",	\
					\"Ok\":\"{}\",	\
				}}	\
			}}",
				join_with_separator(ports.begin(), ports.end(), ","),
				ErrorString[ErrorCode::Ok],
				ErrorString[ErrorCode::Error],
				ErrorString[ErrorCode::PortNotExists],
				ErrorString[ErrorCode::PortClosed]
		);
		return test_data;
	}
}
