#pragma once
#include "main.h"
#include "sqlite3.h"
#include <mutex>

using ConfigList = std::vector<std::string>;

class Model
{
	public:
		Model(const char *filename);
		~Model();
		auto GetConfigurations() const -> ConfigList;
		auto GetConfiguration(const char *name) const -> SerialPortConfig;
		void AddConfiguration(const char *name, SerialPortConfig &PortConfig) const;
		void RenameConfiguration(const char *name, const char *new_name) const;
		void UpdateConfiguration(const char *name, SerialPortConfig &PortConfig) const;
		void DeleteConfiguration(const char *name) const;
		auto GetCommands(const char *configuration) const -> std::vector<Command>;
		void AddCommand(const char *configuration, const char *cmd_name, const char *command) const;
		void UpdateCommand(const char *configuration, const char *cmd_name, const char *command, const char *old_name) const;
		void DeleteCommand(const char *configuration, const char *cmd_name) const;

	private:
		sqlite3 *db;
		mutable std::mutex db_mutex;
		mutable std::mutex cache_mutex;
		mutable ConfigList CachedConfigurations;
		void ResetCache() const;
};
