#pragma once

#include "sqlite3.h"
#include <string>
#include <array>
#include <vector>
#include <map>

using ConfigList = std::vector<std::string>;

struct SerialPortConfig
{
	// std::string Name;
	unsigned int Speed = 115200;
	unsigned int Databits = 8;
	bool Parity = false;
	unsigned int Stopbits = 1;
	bool Flowcontrol = false;
	std::string Format = "ASCII";
};

struct Command
{
	std::string Name = "NULL";
	std::string Cmd = "NULL";
};

class Model
{
	public:
		Model(const char *filename);
		~Model();
		ConfigList GetConfigurations();
		SerialPortConfig GetConfiguration(const char *name);
		void AddConfiguration(const char *name, SerialPortConfig &PortConfig, const bool active = false);
		void UpdateConfiguration(const char *name, SerialPortConfig &PortConfig, const bool active = false);
		void DeleteConfiguration(const char *name);
		std::vector<Command> GetCommands(const char *configuration);
		void AddCommand(const char *configuration, const char *cmd_name, const char *command);
		void UpdateCommand(const char *configuration, const char *cmd_name, const char *command);
		void DeleteCommand(const char *configuration, const char *cmd_name);

	private:
		sqlite3 *db;
};
