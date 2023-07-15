#pragma once
#include <iostream>
#include <array>
#include <string>
#include <vector>
#include <map>

using ConfigList = std::vector<std::string>;

enum class ErrorCode
{
	Ok,
	Error
};

extern std::map<ErrorCode, std::string> ErrorString;

extern std::array<std::string, 3> PortSpeed;
extern std::array<std::string, 2> PortDatabits;
extern std::array<std::string, 3> PortParity;
extern std::array<std::string, 2> PortStopbits;
extern std::array<std::string, 2> PortFlowcontrol;

struct SerialPortConfig
{
	// std::string Name;
	std::string Speed = "115200";
	std::string Databits = "8";
	std::string Parity = "None";
	std::string Stopbits = "1";
	std::string Flowcontrol = "None";
	std::string Format = "ASCII";
};

struct Command
{
	std::string Name = "NULL";
	std::string Cmd = "NULL";
};
