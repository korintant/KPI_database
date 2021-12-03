#include "Model.h"
#include <iostream>
using namespace model;

std::unordered_map<std::string, Model::dataTypes> Model::data_types = {
		{"text",dataTypes::text},
		{"integer",dataTypes::integer},
		{"numeric",dataTypes::numeric},
		{"date",dataTypes::date},
		{"EIT_subject",dataTypes::EIT_subject},
		{"Education_level",dataTypes::Education_level},
		{"Type_of_subject_of_educational_activity",dataTypes::Type_of_subject_of_educational_activity},
		{"Ukraine_region",dataTypes::Ukraine_region},
};

Model::Model(const std::string& _database_name, const std::string& _username, const std::string& _password)
	: database_name(_database_name), username(_username), password(_password) {
	connect();

	auto tablesQueryResult = query(
		"SELECT table_name\n"
		"FROM information_schema.tables\n"
		"WHERE table_type = 'BASE TABLE'\n"
		"AND table_schema = 'public';\n"
	);

	if (PQresultStatus(tablesQueryResult) != PGRES_TUPLES_OK) {
		auto error = std::string(PQresultErrorMessage(tablesQueryResult));
		throw std::logic_error(error);
	}

	for (int i = 0; i < PQntuples(tablesQueryResult); i++) {
		for (int j = 0; j < PQnfields(tablesQueryResult); j++) {
			tables.push_back({ PQgetvalue(tablesQueryResult, i, j) });
		}
	}

}

void Model::connect() {
	//"user=postgres password= host=127.0.0.1 dbname=postgres"
	const std::string connection_string =
		"user=" + username + "\n"
		"password=" + password + "\n"
		"host = 127.0.0.1 dbname=" + database_name + "\n"
		"client_encoding = UTF8";

	connection = PQconnectdb(connection_string.c_str());
	if (connection == nullptr) {
		throw std::logic_error("Can't connect to database!");
	}
}

PGresult* Model::query(const std::string& query) const {
	return PQexec(connection, query.c_str());
}
std::vector<std::string> Model::getTables() const {
	return tables;
}
std::string Model::GetLastError() const {
	auto msg = PQerrorMessage(connection);

	return msg;
}

std::vector<std::pair<std::string, model::Model::dataTypes>> Model::getTableColumns(int table_index) const {
	std::vector<std::pair<std::string, model::Model::dataTypes>> columns;

	auto columns_name_result = query(
		"SELECT column_name\n"
		"FROM information_schema.columns\n"
		"WHERE table_schema = 'public'\n"
		"AND table_name = '" + tables[table_index] + "';"
	);

	auto columns_types_result = query(
		"SELECT data_type\n"
		"FROM   information_schema.columns\n"
		"WHERE table_schema = 'public'\n"
		"AND  table_name = '" + tables[table_index] +"';"
		);

	auto columns_custom_types_result = query(
		"SELECT udt_name\n"
		"FROM   information_schema.columns\n"
		"WHERE table_schema = 'public'\n"
		"AND  table_name = '" + tables[table_index] + "';"
	);


	for (int i = 0; i < PQntuples(columns_name_result); i++) {
		for (int j = 0; j < PQnfields(columns_name_result); j++) {
			auto name = PQgetvalue(columns_name_result, i, j);
			std::string type = PQgetvalue(columns_types_result, i, j);
			if(type == "USER-DEFINED")
				type = PQgetvalue(columns_custom_types_result, i, j);
			if(data_types.count(type))
				columns.push_back({ name, data_types[type] });
			else
				throw std::logic_error("Unrecognizable data type: " + type + "!");
		}
	}

	return columns;
}

std::vector<std::vector<std::string>> Model::getTableRows(int table_index) const {
	std::vector<std::vector<std::string>> rows;

	auto rows_result = query(
		"SELECT *\n"
		"FROM \"" + tables[table_index] + "\";"
	);

	for (int i = 0; i < PQntuples(rows_result); i++) {
		rows.push_back({});
		for (int j = 0; j < PQnfields(rows_result); j++) {
			auto value = PQgetvalue(rows_result, i, j);
			rows[i].push_back(value);
		}
	}

	return rows;
}

std::string Model::getTablePrimaryKey(int tableIndex) const {
	std::string table_name = tables[tableIndex];

	auto* primary_result = query(
		"SELECT a.attname, format_type(a.atttypid, a.atttypmod) AS data_type\n"
		"FROM pg_index i\n"
		"JOIN pg_attribute a ON a.attrelid = i.indrelid\n"
		"AND a.attnum = ANY(i.indkey)\n"
		"WHERE i.indrelid = 'public.\"" + table_name +
		"\"'::regclass\n"
		"AND i.indisprimary;"
	);

	if (PQresultStatus(primary_result) == PGRES_TUPLES_OK) {
		return PQgetvalue(primary_result, 0, 0);
	}
	return "";
}

std::unordered_map<std::string, std::pair<std::string, std::string>> Model::getTableForeignKeys(int tableIndex) const {
	std::string table_name = tables[tableIndex];

	std::unordered_map<std::string, std::pair<std::string, std::string>> res;

	auto* foreign_result = query(
		"SELECT kcu.column_name, ccu.table_name AS foreign_table_name, ccu.column_name AS foreign_column_name\n"
		"FROM information_schema.table_constraints AS tc\n"
		"JOIN information_schema.key_column_usage AS kcu\n"
		"ON tc.constraint_name = kcu.constraint_name\n"
		"AND tc.table_schema = kcu.table_schema\n"
		"JOIN information_schema.constraint_column_usage AS ccu\n"
		"ON ccu.constraint_name = tc.constraint_name\n"
		"AND ccu.table_schema = tc.table_schema\n"
		"WHERE tc.constraint_type = 'FOREIGN KEY' AND tc.table_name = '" + table_name + "';"
	);

	for (int i = 0; i < PQntuples(foreign_result); i++) {
		auto column = PQgetvalue(foreign_result, i, 0);
		auto foreign_table = PQgetvalue(foreign_result, i, 1);
		auto foreign_column = PQgetvalue(foreign_result, i, 2);
		res[column] = { foreign_table, foreign_column };
	}

	if (PQresultStatus(foreign_result) != PGRES_TUPLES_OK) {
		std::cout << "Error : " << GetLastError() << std::endl;
	}

	return res;
}
