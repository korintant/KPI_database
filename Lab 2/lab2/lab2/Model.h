#pragma once
#include <vector>
#include <string>
#include <unordered_map>
#include "libpq-fe.h"
#include "Model.h"
namespace model {
	class Model
	{
	public:
		enum class dataTypes {
			text,
			integer,
			numeric,
			date,
			EIT_subject,
			Education_level,
			Type_of_subject_of_educational_activity,
			Ukraine_region,
		};
		static std::unordered_map<std::string, dataTypes> data_types;


		Model(const std::string& _database_name, const std::string& _username, const std::string& _password);
		PGresult* query(const std::string& query) const;
		std::vector<std::string> getTables() const;
		std::string getTablePrimaryKey(int tableIndex) const;
		std::unordered_map<std::string, std::pair<std::string, std::string>> getTableForeignKeys(int tableIndex) const;
		std::vector<std::vector<std::string>> getTableRows(int table_index) const;
		std::vector<std::pair<std::string, model::Model::dataTypes>> getTableColumns(int tableIndex) const;
		std::string GetLastError() const;

	private:
		void connect();

		PGconn* connection = nullptr;

		std::vector<std::string> tables;

		std::string username;
		std::string database_name;
		std::string password;
	};
}

