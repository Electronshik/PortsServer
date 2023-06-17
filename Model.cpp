#include "Model.h"
#include <format>
#include <iostream>

Model::Model(const char *filename)
{
	int rc = sqlite3_open(filename, &(this->db));
	if (rc)
	{
		printf("Can't open database: %s\n", sqlite3_errmsg(db));
	}
	else
	{
		printf("Opened database successfully\n");
		std::string create_configurations_query = "CREATE TABLE configurations (id INTEGER PRIMARY KEY AUTOINCREMENT,	\
			name TEXT NOT NULL UNIQUE, speed INTEGER, databits INTEGER, parity INTEGER, stopbits INTEGER, flowcontrol INTEGER, format TEXT, active INTEGER)";

		sqlite3_exec(this->db, create_configurations_query.c_str(), NULL, 0, NULL);
	}
}

Model::~Model()
{
	sqlite3_close(this->db);
}

template<typename T, typename V>
int sql_callback(void *data, int argc, char **argv, char **azColName)
{
	// for (size_t i = 0; i < argc; i++)
	// 	printf("i: %d, %s = %s\n", i, azColName[i], argv[i] ? argv[i] : "NULL");

	T t;
	t((V*)data, argc, azColName, argv);
	return 0;
}

ConfigList Model::GetConfigurations()
{
	ConfigList config_list;

	auto clbk = [](ConfigList *result, int argc, char **azColName, char **argv) {
		for (size_t i = 0; i < argc; i++)
		{
			// printf("Col: %s : %s\n", azColName[i], argv[i]);
			if (strcmp(azColName[i], "name") == 0)
			{
				((ConfigList*)result)->emplace_back(argv[i]);
			}
		}
	};

	std::string query = "SELECT name FROM configurations";
	sqlite3_exec(this->db, query.c_str(), sql_callback<decltype(clbk), ConfigList>, &config_list, NULL);
	return config_list;
}

SerialPortConfig Model::GetConfiguration(const char *name)
{
	SerialPortConfig port_config;

	auto clbk = [](SerialPortConfig *result, int argc, char **azColName, char **argv) {
		for (size_t i = 0; i < argc; i++)
		{
			// std::string param = argv[i];
			// printf("Col: %s : %s\n", azColName[i], argv[i]);
			if (strcmp(azColName[i], "speed") == 0)
			{
				result->Speed = std::atoi(argv[i]);
			}
			else if (strcmp(azColName[i], "databits") == 0)
			{
				result->Databits = std::atoi(argv[i]);
			}
			else if (strcmp(azColName[i], "parity") == 0)
			{
				result->Parity = std::atoi(argv[i]);
			}
			else if (strcmp(azColName[i], "stopbits") == 0)
			{
				result->Stopbits = std::atoi(argv[i]);
			}
			else if (strcmp(azColName[i], "flowcontrol") == 0)
			{
				result->Flowcontrol = std::atoi(argv[i]);
			}
		}
	};

	std::string query = std::format("SELECT * FROM configurations WHERE name='{}'", name);
	sqlite3_exec(this->db, query.c_str(), sql_callback<decltype(clbk), SerialPortConfig>, &port_config, NULL);
	return port_config;
}

void Model::AddConfiguration(const char *name, SerialPortConfig &PortConfig, const bool active)
{
	std::string query = std::format("INSERT INTO configurations (name, speed, databits, parity, stopbits, flowcontrol, format, active)	\
		VALUES('{0}','{1}','{2}','{3:d}','{4}','{5:d}','{6}','{7:d}')",
		name, PortConfig.Speed, PortConfig.Databits, PortConfig.Parity,
		PortConfig.Stopbits, PortConfig.Flowcontrol, PortConfig.Format.c_str(), active);
	std::cout << query << std::endl;
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);

	query = std::format("CREATE TABLE {}(id INTEGER PRIMARY KEY AUTOINCREMENT,	\
		name TEXT NOT NULL UNIQUE, command TEXT)", name);

	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);
}

void Model::UpdateConfiguration(const char *name, SerialPortConfig &PortConfig, const bool active)
{
	std::string query = std::format("UPDATE configurations SET speed='{0}', databits={1}, parity={2:d},	\
		stopbits={3}, flowcontrol={4:d}, format='{5}', active={6:d} WHERE name='{7}'",
		PortConfig.Speed, PortConfig.Databits, PortConfig.Parity,
		PortConfig.Stopbits, PortConfig.Flowcontrol, PortConfig.Format.c_str(), active, name);
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);
}

void Model::DeleteConfiguration(const char *name)
{
	std::string query = std::format("DELETE FROM configurations	WHERE name='{}'", name);
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);

	query = std::format("DROP TABLE {}", name);
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);
}

std::vector<Command> Model::GetCommands(const char *configuration)
{
	std::vector<Command> commands_list;

	auto clbk = [](std::vector<Command> *result, int argc, char **azColName, char **argv) {
		Command command;
		for (size_t i = 0; i < argc; i++)
		{
			// printf("Col: %s : %s\n", azColName[i], argv[i]);
			if (strcmp(azColName[i], "name") == 0)
			{
				command.Name = argv[i];
			}
			else if (strcmp(azColName[i], "command") == 0)
			{
				command.Cmd = argv[i];
			}
		}
		result->push_back(command);
	};

	std::string query = std::format("SELECT id, name, command FROM {}", configuration);
	sqlite3_exec(this->db, query.c_str(), sql_callback<decltype(clbk), std::vector<Command>>, &commands_list, NULL);
	return commands_list;
}

void Model::AddCommand(const char *configuration, const char *cmd_name, const char *command)
{
	std::string query = std::format("INSERT INTO {0}(name, command) VALUES('{1}', '{2}')", configuration, cmd_name, command);
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);
}

void Model::UpdateCommand(const char *configuration, const char *cmd_name, const char *command)
{
	std::string query = std::format("UPDATE {0} SET command='{1}' WHERE name='{2}'", configuration, command, cmd_name);
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);
}

void Model::DeleteCommand(const char *configuration, const char *cmd_name)
{
	std::string query = std::format("DELETE FROM {0} WHERE name='{1}'", configuration, cmd_name);
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);	
}
