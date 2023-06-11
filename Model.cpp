#include "Model.h"
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
		std::string create_configurations_query = "CREATE TABLE configurations (	\
			id INTEGER PRIMARY KEY AUTOINCREMENT,	\
			name TEXT NOT NULL UNIQUE,	\
			param TEXT	\
		)";

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
	T t;
	for (size_t i = 0; i < argc; i++)
	{
		// printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
		t((V*)data, azColName[i], argv[i]);
	}
	return 0;
};

std::vector<std::string> Model::GetConfigurations()
{
	std::vector<std::string> config_list;

	auto clbk = [](std::vector<std::string> *result, char *name, char *param) {
		// printf("Lam: %s : %s\n", name, param);
		((std::vector<std::string>*)result)->emplace_back(param);
	};

	std::string query = "SELECT name FROM configurations";
	sqlite3_exec(this->db, query.c_str(), sql_callback<decltype(clbk), std::vector<std::string>>, &config_list, NULL);
	return config_list;
}

void Model::AddConfiguration(const char *name)
{
	std::string add_configuration_query = std::string("INSERT INTO configurations (name, param) VALUES('") + name + "', '')";
	sqlite3_exec(this->db, add_configuration_query.c_str(), NULL, 0, NULL);

	std::string add_configuration_commands_query = std::string("CREATE TABLE ") + name + "(	\
		id INTEGER PRIMARY KEY AUTOINCREMENT,	\
		name TEXT NOT NULL UNIQUE,	\
		command TEXT	\
	)";

	sqlite3_exec(this->db, add_configuration_commands_query.c_str(), NULL, 0, NULL);
}

void Model::DeleteConfiguration(const char *name)
{
	std::string query = std::string("DELETE FROM configurations WHERE name='") + name + "'";
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);

	query = std::string("DROP TABLE ") + name;
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);
}

void Model::AddCommand(const char *configuration, const char *cmd_name, const char *command)
{
	std::string query = std::string("INSERT INTO ") + configuration + "(name, command) VALUES('" + cmd_name + "', '" + command + "')";
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);
}

void Model::UpdateCommand(const char *configuration, const char *cmd_name, const char *command)
{
	std::string query = std::string("UPDATE ") + configuration + "SET command='" + command + "' WHERE name='" + cmd_name + "'";
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);
}

void Model::DeleteCommand(const char *configuration, const char *cmd_name)
{
	std::string query = std::string("DELETE FROM ") + configuration + " WHERE name='" + cmd_name + "'";
	sqlite3_exec(this->db, query.c_str(), NULL, 0, NULL);	
}
