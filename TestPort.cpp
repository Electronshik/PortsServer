#include "TestPort.h"

auto TestPort::GetPortsList() -> std::vector<std::string>
{
	std::vector<std::string> result;
	result.push_back("COM85");
	result.push_back("COM88");
	return result;
}

TestPort::TestPort(std::string &name, SerialPortConfig &port_config)
{
	this->name = name;
}

TestPort::~TestPort()
{

}

auto TestPort::GetName() -> std::string
{
	return this->name;
}

void TestPort::Write(char* buff, int len)
{
	this->received.assign(buff);
}

auto TestPort::Read(char* buff) -> int
{
	// char test_buff[32] = "Test String";
	int len = this->received.length();

	strcpy(buff, this->received.c_str());
	this->received.clear();
		
	return len;	
}
