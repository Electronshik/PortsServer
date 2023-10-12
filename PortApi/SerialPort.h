#pragma once
#include "main.h"

#include <windows.h>

class SerialPort
{
	public:
		static auto GetPortsList() -> std::vector<std::string>;
		SerialPort(std::string &name, SerialPortConfig &port_config);
		~SerialPort();
		auto GetName() -> std::string;
		void Write(char* buff, int len);
		auto Read(char* buff) -> int;

	private:
		std::string name;
		HANDLE port;
		DCB config;
};
