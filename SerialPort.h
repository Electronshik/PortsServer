#pragma once
#include "main.h"

#include <windows.h>

class SerialPort
{
	public:
		static std::vector<std::string> GetPortsList();
		SerialPort(std::string &name, SerialPortConfig &port_config);
		~SerialPort();
		void Write(char* buff, int len);
		int Read(char* buff);

	private:
		HANDLE port;
		DCB config;
};
