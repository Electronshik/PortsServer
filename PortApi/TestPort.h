#pragma once
#include "main.h"
#include "SerialPort.h"

class TestPort
{
	public:
		static auto GetPortsList() -> std::vector<std::string>;
		TestPort(std::string &name, SerialPort::Config &serial_config);
		~TestPort();
		auto GetName() -> std::string;
		void Write(char* buff, int len);
		auto Read(char* buff) -> unsigned int;

	private:
		std::string name;
		std::string received;
};
