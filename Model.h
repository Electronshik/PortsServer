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

	private:
		sqlite3 *db;
};
