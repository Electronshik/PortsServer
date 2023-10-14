#include "TestPort.h"

auto TestPort::GetPortsList() -> std::vector<std::string>
{
	std::vector<std::string> result;
	result.push_back("COM85");
	result.push_back("COM88");
	return result;
}

TestPort::TestPort(std::string &name, SerialPort::Config &serial_config)
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
	this->received.append(buff);
}

auto TestPort::Read(char* buff) -> unsigned int
{
	unsigned int len = this->received.length();
	strcpy(buff, this->received.c_str());
	this->received.clear();

	return len;	
}
