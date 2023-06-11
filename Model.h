#pragma once

#include "sqlite3.h"
#include <string>
#include <array>
#include <vector>

class Model
{
	public:
		Model(const char *filename);
		~Model();
		std::vector<std::string> GetConfigurations();
		void AddConfiguration(const char *name);
		void DeleteConfiguration(const char *name);
		void AddCommand(const char *configuration, const char *cmd_name, const char *command);
		void UpdateCommand(const char *configuration, const char *cmd_name, const char *command);
		void DeleteCommand(const char *configuration, const char *cmd_name);

	private:
		sqlite3 *db;
};
