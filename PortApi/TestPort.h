#pragma once
#include "main.h"

class TestPort
{
	public:
		static auto GetPortsList() -> std::vector<std::string>;
		TestPort(std::string &name, SerialPortConfig &port_config);
		~TestPort();
		auto GetName() -> std::string;
		void Write(char* buff, int len);
		auto Read(char* buff) -> int;

	private:
		std::string name;
		std::string received;
};
