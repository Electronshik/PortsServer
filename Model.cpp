#include "Model.h"

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

	// template<typename T>
	auto clbk = [](std::vector<std::string> *result, char *name, char *param) {
		// printf("Lam: %s : %s", name, param);
		((std::vector<std::string>*)result)->emplace_back(param);
	};

	std::string query = "SELECT name FROM sqlite_schema WHERE type='table' AND name NOT LIKE 'sqlite_%'";
	sqlite3_exec(this->db, query.c_str(), sql_callback<decltype(clbk), std::vector<std::string>>, &config_list, NULL);
	return config_list;
}

void Model::AddConfiguration(const char *name)
{
	std::string create_table_query = "CREATE TABLE ";

	create_table_query.append(name);
	create_table_query.append("(	\
		id INT PRIMARY KEY NOT NULL,	\
		name TEXT NOT NULL,	\
		param TEXT	\
	)");

	sqlite3_exec(this->db, create_table_query.c_str(), NULL, 0, NULL);
}
