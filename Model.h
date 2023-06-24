#pragma once
#include "main.h"
#include "sqlite3.h"
#include <array>
#include <map>

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
		void AddConfiguration(const char *name, SerialPortConfig &PortConfig);
		void UpdateConfiguration(const char *name, SerialPortConfig &PortConfig);
		void DeleteConfiguration(const char *name);
		std::vector<Command> GetCommands(const char *configuration);
		void AddCommand(const char *configuration, const char *cmd_name, const char *command);
		void UpdateCommand(const char *configuration, const char *cmd_name, const char *command);
		void DeleteCommand(const char *configuration, const char *cmd_name);

	private:
		sqlite3 *db;
};
