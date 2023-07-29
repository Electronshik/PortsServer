#pragma once
#include "main.h"
#include "sqlite3.h"

class Model
{
	public:
		Model(const char *filename);
		~Model();
		auto GetConfigurations() -> ConfigList;
		auto GetConfiguration(const char *name) -> SerialPortConfig;
		void AddConfiguration(const char *name, SerialPortConfig &PortConfig);
		void RenameConfiguration(const char *name, const char *new_name);
		void UpdateConfiguration(const char *name, SerialPortConfig &PortConfig);
		void DeleteConfiguration(const char *name);
		auto GetCommands(const char *configuration) -> std::vector<Command>;
		void AddCommand(const char *configuration, const char *cmd_name, const char *command);
		void UpdateCommand(const char *configuration, const char *cmd_name, const char *command);
		void DeleteCommand(const char *configuration, const char *cmd_name);

	private:
		sqlite3 *db;
};
