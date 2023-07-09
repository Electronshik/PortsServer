#pragma once
#include "main.h"

#include <windows.h>

class SerialPort
{
	public:
		static std::vector<std::string> GetPortsList();
		SerialPort(std::string &name, SerialPortConfig &port_config);
		~SerialPort();
		std::string GetName();
		void Write(char* buff, int len);
		int Read(char* buff);

	private:
		std::string name;
		HANDLE port;
		DCB config;
};
