#include "SerialPortApi.h"
#include "SerialPort.h"
#include "TestPort.h"
#include <memory>
#include <concepts>
#include <format>

import Utils;

namespace SerialPortApi
{
	template<class T> concept PortTypeInterface =
	requires (T t)
	{
		{ T::GetPortsList() } -> std::same_as<std::vector<std::string>>;
		T(std::declval<std::string&>(), std::declval<SerialPort::Config&>());
		// std::is_constructible<T>; // cease to work in mvsc
		// std::is_destructible<T>; // cease to work in mvsc
		{ t.GetName() } -> std::same_as<std::string>;
		{ t.Write(std::declval<char*>(), int{}) } -> std::same_as<void>;
		{ t.Read(std::declval<char*>()) } -> std::same_as<unsigned int>;
	};

	// using PortType = SerialPort;
	using PortType = TestPort;
	static_assert(PortTypeInterface<PortType>);

	std::vector<std::unique_ptr<PortType>> OpenedPorts;
	std::map<ErrorCode, std::string> ErrorString = {
		{ ErrorCode::Ok, "Ok" },
		{ ErrorCode::Error, "Error" },
		{ ErrorCode::PortNotExists, "PortNotExists" },
		{ ErrorCode::PortClosed, "PortClosed" },
	};

	void StringPortConfigToSerialConfig (SerialPortConfig &port_config, SerialPort::Config &serial_config)
	{
		if (port_config.Speed == "9600")
			serial_config.Speed = SerialPort::Speed::s_9600;
		else if (port_config.Speed == "19200")
			serial_config.Speed = SerialPort::Speed::s_19200;

		if (port_config.Databits == "7")
			serial_config.Databits = SerialPort::Databits::b7;

		if (port_config.Parity == "Odd")
			serial_config.Parity = SerialPort::Parity::Odd;
		else if (port_config.Parity == "Even")
			serial_config.Parity = SerialPort::Parity::Even;

		if (port_config.Stopbits == "2")
			serial_config.Stopbits = SerialPort::Stopbits::b2;

		if (port_config.Flowcontrol == "Yes")
			serial_config.Flowctrl = SerialPort::Flowctrl::Yes;
	}

	auto GetPortsList() -> std::vector<std::string>
	{
		return PortType::GetPortsList();
	}

	auto OpenPort(std::string &port_name, SerialPortConfig &port_config) -> ErrorCode
	{
		SerialPort::Config serial_config;
		StringPortConfigToSerialConfig(port_config, serial_config);

		OpenedPorts.push_back(std::make_unique<PortType>(port_name, serial_config));
		return ErrorCode::Ok;
	}

	auto ClosePort(std::string &port_name) -> ErrorCode
	{
		auto it = std::find_if(OpenedPorts.begin(), OpenedPorts.end(),
			[&](std::unique_ptr<PortType>& ptr){ return ptr->GetName() == port_name; });

		if (it != OpenedPorts.end())
		{
			OpenedPorts.erase(it);
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
			\"ports\":[{}],	\
				\"errors\":{{	\
					\"Ok\":\"{}\",	\
					\"Error\":\"{}\",	\
					\"PortNotExists\":\"{}\",	\
					\"PortClosed\":\"{}\"	\
				}}	\
			}}",
				join_wrap_with_sep(ports.begin(), ports.end(), ",", "\""),
				ErrorString[ErrorCode::Ok],
				ErrorString[ErrorCode::Error],
				ErrorString[ErrorCode::PortNotExists],
				ErrorString[ErrorCode::PortClosed]
		);
		return test_data;
	}
}
