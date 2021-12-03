#include "Controller.h"
#include <chrono>
#include <iostream>
#include <vector>

using namespace controller;
using std::cin;
using std::cout;
using std::endl;

Controller::Controller(std::shared_ptr<model::Model> _model, std::shared_ptr<view::View> _view) : model(_model), view(_view) {
	action_map[static_cast<int>(action::insert)] = { "insert", &Controller::performInsert };
	action_map[static_cast<int>(action::remove)] = { "remove", &Controller::performRemove };
	action_map[static_cast<int>(action::edit)] = { "edit", &Controller::performEdit };
	action_map[static_cast<int>(action::generate)] = { "generate", &Controller::performGenerate };
	action_map[static_cast<int>(action::search)] = { "search", &Controller::performSearch };
	action_map[static_cast<int>(action::print)] = { "print", &Controller::performPrint };
	action_map[static_cast<int>(action::drop)] = { "drop", &Controller::performDrop };
	action_map[static_cast<int>(action::end)] = { "end", nullptr };


};

void Controller::interactionLoop() {
	bool go_on = true;
	try {
		while (go_on) {
			cout << "Choose an option" << endl;
			for (auto& action : action_map) {
				cout << static_cast<int>(action.first) << " " << action.second.first << endl;
			}
			int input;
			cin >> input;

			auto action = action_map.find(input);

			if (action == action_map.end()) {
				cout << "Invalid input! Please, input valid option" << endl;
				continue;
			}

			if (input == static_cast<int>(action::end)) {
				go_on = false;
				cout << "Goodbye!" << endl;
			}
			else {
				(this->*(action->second.second))(true);
			}

		}
	}
	catch (const std::exception& e) {
		cout << "Exception occured: " << e.what() << endl;
	}
}

std::pair<int, string> Controller::chooseTable() {

	auto tables = model->getTables();

	if (tables.empty()) {
		cout << "There are no tables, nothing to choose from" << endl;
		return { -1, "" };
	}
	cout << "Choose table to use" << endl;
	for (int i = 0; i < tables.size(); ++i)
		cout << i + 1 << " " << tables[i] << endl;
	int input = -1;
	while (input == -1) {
		cin >> input;
		if (input > tables.size() || input < 0) {
			input = -1;
			cout << "Invalid table. Choose a table from the list above or enter 0 to break" << endl;
		}

	}

	return { input - 1, input?tables[input - 1]:"" };
}

int Controller::chooseRow(std::pair<int, string> table) {

	auto rows = model->getTableRows(table.first);
	auto cols = model->getTableColumns(table.first);

	if (rows.empty()) {
		cout << "There are no rows, nothing to choose from" << endl;
		return -1;
	}
	int input = -1;
	if (rows.size() < 100)
		input = 1;
	else {
		cout << "Choose which rows to display" << endl;
		cout << "1 All rows" << endl;
		cout << "2 First 100 rows" << endl;
		cout << "3 Last 100 rows" << endl;
		while (input == -1) {
			cin >> input;
			if (input > 3 || input < 1) {
				input = -1;
				cout << "Invalid option. Choose an option from the list above" << endl;
			}

		}
	}
	int left = 0;
	int right = rows.size();

	if(input == 2)
		right = std::min(100, (int)(rows.size()));
	if(input == 3)
		left = std::max(0, (int)(rows.size()) - 100);

	auto result = model->query("SELECT * FROM public.\"" + table.second + "\"");

	view->showTable(result, left, right, true);
	cout << "Choose a row:" << endl;

	input = -1;
	while (input == -1) {
		std::cin >> input;
		if (input > rows.size() || input < 0) {
			input = -1;
			cout << "Invalid row. Choose a row from the list above or enter 0 to break";
		}
	}
	return input - 1;
}

std::string Controller::getValue(string& column_name, model::Model::dataTypes type, bool skippable) {
	cout << "Enter value for column ";
	cout << column_name;
	if (skippable)
		cout << " or enter \"SKIP\" to skip this column";
	cout << endl;

	string value = "";

	string add = "";
	if (type == model::Model::dataTypes::text || 
		type == model::Model::dataTypes::EIT_subject ||
		type == model::Model::dataTypes::Type_of_subject_of_educational_activity ||
		type == model::Model::dataTypes::Ukraine_region ||
		type == model::Model::dataTypes::Education_level)
	{
		add += "'";
	}

	getline(cin, value);

	if (skippable && value == "SKIP")
		return "SKIP\n\n";
	return add + value + add;
}

PGresult* Controller::performInsert(bool to_print) {
	std::pair<int, string> table = chooseTable();
	if (table.first == -1) {
		cout << "Insert interapted" << endl;
		return nullptr;
	}

	auto columns = model->getTableColumns(table.first);
	
	string tmp;
	getline(cin, tmp);
	
	std::vector<string> new_row;
	for (auto& column : columns) {
		new_row.push_back(getValue(column.first, column.second));
	}

	string query_new_row = "(";
	for (int i = 0; i < (int)(new_row.size()) - 1; i++)
		query_new_row += new_row[i] + ", ";
	if (new_row.size())
		query_new_row += new_row[new_row.size() - 1];
	query_new_row += ")";

	string query_insert = "INSERT INTO \"" + table.second + "\"\n" +
						 "VALUES " + query_new_row + "\n";

	auto result = model->query(query_insert);
	if (to_print)
		view->printInsert(result, model->GetLastError());
	return result;
}
PGresult* Controller::performRemove(bool to_print) {
	std::pair<int, string> table = chooseTable();
	if (table.first == -1) {
		cout << "Remove interapted" << endl;
		return nullptr;
	}
	int row_index = chooseRow(table);
	if (row_index == -1) {
		cout << "Remove interapted" << endl;
		return nullptr;
	}


	auto columns = model->getTableColumns(table.first);
	auto rows = model->getTableRows(table.first);
	auto prime_key = model->getTablePrimaryKey(table.first);

	int prime_key_index = columns.size();

	for (int i = 0; i < columns.size(); i++)
		if (columns[i].first == prime_key) {
			prime_key_index = i;
			break;
		}

	string k;

	string query_remove = "DELETE FROM public.\"" + table.second + "\"\n"
		"WHERE \"" + columns[prime_key_index].first + "\" = " + rows[row_index][prime_key_index];

	auto result = model->query(query_remove);
	if (to_print)
		view->printRemove(result, model->GetLastError());
	return result;
}
PGresult* Controller::performEdit(bool to_print) {
	std::pair<int, string> table = chooseTable();
	if (table.first == -1) {
		cout << "Edit interapted" << endl;
		return nullptr;
	}
	int row_index = chooseRow(table);
	if (row_index == -1) {
		cout << "Edit interapted" << endl;
		return nullptr;
	}


	auto columns = model->getTableColumns(table.first);
	auto rows = model->getTableRows(table.first);
	auto prime_key = model->getTablePrimaryKey(table.first);

	int prime_key_index = columns.size();

	for(int i = 0; i < columns.size(); i++)
		if (columns[i].first == prime_key) {
			prime_key_index = i;
			break;
		}

	string query_edit = "UPDATE public.\"" + table.second + "\"\n";

	query_edit += "SET ";

	string tmp;
	getline(cin, tmp);

	bool first = true;
	for (int i = 0; i < columns.size(); i++) {
		string val = getValue(columns[i].first, columns[i].second, true);
		if (val != "SKIP\n\n") {
			if (!first)
				query_edit += ", ";
			first = false;
			query_edit += "\"" + columns[i].first + "\" = " + val;
		}
	}

	if (first) {
		cout << "None of the column was modified" << endl;
		return nullptr;
	}

	query_edit +=	"\nWHERE \"" + columns[prime_key_index].first + "\" = " + rows[row_index][prime_key_index];

	auto result = model->query(query_edit);
	if (to_print)
		view->printEdit(result, model->GetLastError());
	return result;
}
PGresult* Controller::performGenerate(bool to_print) {
	std::pair<int, string> table = chooseTable();
	if (table.first == -1) {
		cout << "Generate interapted" << endl;
		return nullptr;
	}

	auto columns = model->getTableColumns(table.first);
	auto rows = model->getTableRows(table.first);
	auto foreign_keys = model->getTableForeignKeys(table.first);
	auto prime_key = model->getTablePrimaryKey(table.first);

	auto generatingStringFromType = [&](model::Model::dataTypes dt) -> string {
		switch (dt) {
		case model::Model::dataTypes::integer:
			return "(RANDOM() * 40)::integer + 1990";
		case model::Model::dataTypes::numeric:
			return "(RANDOM() * 100)::numeric";
		case model::Model::dataTypes::text:
			return "SUBSTR(MD5(RANDOM()::text), 3, 10)";
		case model::Model::dataTypes::date:
			return "timestamp '2010-01-10 20:00:00' + random() * (timestamp '2030-01-10 20:00:00' - timestamp '2010-01-10 20:00:00')";
		case model::Model::dataTypes::EIT_subject:
			return "(enum_range(NULL::\"EIT_subject\"))[1 + FLOOR((RANDOM() * 12))::integer]";
		case model::Model::dataTypes::Education_level:
			return "(enum_range(NULL::\"Education_level\"))[1 + FLOOR((RANDOM() * 5))::integer]";
		case model::Model::dataTypes::Type_of_subject_of_educational_activity:
			return "(enum_range(NULL::\"Type_of_subject_of_educational_activity\"))[1 + FLOOR((RANDOM() * 3))::integer]";
		case model::Model::dataTypes::Ukraine_region:
			return "(enum_range(NULL::\"Ukraine_region\"))[1 + FLOOR((RANDOM() * 25))::integer]";
		default:
			return "";
		}
	};

	std::vector<string> generating_strings;

	for (auto& column : columns) {
		if (foreign_keys.count(column.first))
			generating_strings.push_back(
				"(ARRAY ((SELECT \"" + foreign_keys[column.first].second + "\" FROM public.\"" + foreign_keys[column.first].first + "\")))[1 + FLOOR((RANDOM() * "
				"(SELECT COUNT(\"" + foreign_keys[column.first].second + "\") FROM public.\"" + foreign_keys[column.first].first + "\")"
				"))::integer]"
			);
		else if (prime_key == column.first)
			generating_strings.push_back("(RANDOM() * 1000000)::integer");
		else
			generating_strings.push_back(generatingStringFromType(column.second));
	}

	string generate_query = "INSERT INTO public.\"" + table.second + "\"\nSelect";

	for (int i = 0; i < generating_strings.size(); i++) {
		generate_query += "\n" + generating_strings[i];
		if(i != generating_strings.size() - 1)
			generate_query += ",";
	}
	
	cout << "Enter number of rows to generate" << endl;
	int number;
	cin >> number;
	generate_query += "\nFROM generate_series(1," + std::to_string(number) + ")\nON CONFLICT DO NOTHING";
	auto result = model->query(generate_query);
	if (to_print)
		view->printGenerate(result, model->GetLastError());
	return result;
}
PGresult* Controller::performSearch(bool to_print) {
	cout << "Choose an option:" << endl;
	cout << "1 Find all individuals with Math EIT certificate score higher than X in year Y" << endl;
	cout << "2 Find all active student tickets given by choosen subject" << endl;
	cout << "3 Find subject most popular educational programs" << endl;

	int input = -1;
	while (input == -1) {
		cin >> input;
		if (input > 3 || input < 1) {
			input = -1;
			cout << "Invalid option. Choose an option from the list above" << endl;
		}

	}

	int x, y;
	string s;
	string search_query;
	switch (input)
	{
	case 1:
		cout << "Enter score" << endl;
		cin >> x;
		cout << "Enter year" << endl;
		cin >> y;
		search_query =
			"SELECT *  \n"
			"FROM public.\"Individual\"\n"
			"WHERE \"ID\" IN (\n"
			"SELECT \"Individual_ID\" FROM public.\"EIT_certificate\"\n"
			"WHERE \"Subject\" = 'Math' and \"Result\" >= " + std::to_string(x) + " and \"Year\" = " + std::to_string(y) + ")\n"
			"ORDER BY \"ID\"\n"
			;
		break;
	case 2:
		cout << "Enter subject name" << endl;
		getline(cin, s);
		getline(cin, s);
		search_query =
			"SELECT * FROM public.\"Student_ticket\""
			"WHERE \"Start_date\" <= current_date and \"End_date\" >= current_date and \"Subject_ID\" IN ("
			"SELECT \"ID\" FROM public.\"Subject_of_education_activity\""
			"WHERE \"Name\" = '" + s + "')"
			"ORDER BY \"ID\"\n"
			;
		break;
	case 3:
		cout << "Enter subject name" << endl;
		getline(cin, s);
		getline(cin, s);
		cout << "Enter number of programs" << endl;
		cin >> x;
		search_query =
			"SELECT \"ID\", \"Level_of_education\", \"Education_program\", \"count\" FROM\n"
			"public.\"Education\"\n"
			"INNER JOIN(\n"
			"SELECT \"Education_ID\", COUNT(*) FROM(\n"
			"SELECT \"Education_ID\" FROM\n"
			"(SELECT \"Subject_ID\", \"Education_ID\" FROM public.\"Student_ticket\"\n"
			"UNION ALL\n"
			"SELECT \"Subject_ID\", \"Education_ID\" FROM public.\"Education_document\") AS combined_table\n"
			"WHERE \"Subject_ID\" IN(\n"
			"SELECT \"ID\" FROM public.\"Subject_of_education_activity\"\n"
			"WHERE \"Name\" = '" + s +"')\n"
			") AS countable_table\n"
			"GROUP BY \"Education_ID\"\n"
			"ORDER BY COUNT(*) DESC\n"
			"LIMIT " + std::to_string(x) + ") as count_table\n"
			"ON \"ID\" = \"Education_ID\"\n"
			"ORDER BY \"count\" DESC";
		break;
	default:
		break;
	}

	auto start = std::chrono::steady_clock::now();
	auto result = model->query(search_query);
	auto end = std::chrono::steady_clock::now();

	if (to_print)
		view->printSearch(result, model->GetLastError());

	cout << "Time spent on search: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " ms " << endl;
	return result;
}
PGresult* Controller::performPrint(bool to_print) {
	std::pair<int, string> table = chooseTable();
	if (table.first == -1) {
		cout << "Generate interapted" << endl;
		return nullptr;
	}
	auto rows = model->getTableRows(table.first);

	int input = -1;
	if (rows.size() < 100)
		input = 1;
	else {
		cout << "Choose which rows to display" << endl;
		cout << "1 All rows" << endl;
		cout << "2 First 100 rows" << endl;
		cout << "3 Last 100 rows" << endl;
		while (input == -1) {
			cin >> input;
			if (input > 3 || input < 1) {
				input = -1;
				cout << "Invalid option. Choose an option from the list above" << endl;
			}

		}
	}
	int left = 0;
	int right = rows.size();

	if (input == 2)
		right = std::min(100, (int)(rows.size()));
	if (input == 3)
		left = std::max(0, (int)(rows.size()) - 100);

	auto result = model->query("SELECT * FROM public.\"" + table.second + "\"");

	if(to_print)
		view->showTable(result, left, right, true);

	return result;
}
PGresult* Controller::performDrop(bool to_print) {
	std::pair<int, string> table = chooseTable();
	if (table.first == -1) {
		cout << "Generate interapted" << endl;
		return nullptr;
	}

	string drop_query = "DELETE FROM public.\"" + table.second + "\"";
	auto result = model->query(drop_query);
	if (to_print)
		view->printDrop(result, model->GetLastError());
	return nullptr;
}
