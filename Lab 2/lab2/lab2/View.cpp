#include "View.h"
#include <iostream>
#include <iomanip>
using namespace view;
using std::cout;
using std::endl;

View::View() {

}
void View::printInsert(PGresult* res, const std::string& error) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cout << "Insert ended with an " << error << endl;
	}
	else {
		cout << "Row was succesfully inserted in table!" << endl;
	}
}
void View::printRemove(PGresult* res, const std::string& error) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cout << "Remove ended with an " << error << endl;
	}
	else {
		cout << "Row was succesfully deleted from table!" << endl;
	}
}
void View::printEdit(PGresult* res, const std::string& error) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cout << "Edit ended with an " << error << endl;
	}
	else {
		cout << "Row was successfully edited!" << endl;
	}
}
void View::printGenerate(PGresult* res, const std::string& error) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cout << "Generating ended with an " << error << endl;
	}
	else {
		cout << "Rows were successfully generated!" << endl;
	}
}
void View::printDrop(PGresult* res, const std::string& error) const {
	if (PQresultStatus(res) != PGRES_COMMAND_OK) {
		cout << "Drop ended with an " << error << endl;
	}
	else {
		cout << "Rows were successfully dropped!" << endl;
	}
}
void View::printSearch(PGresult* res, const std::string& error) const {
	if (error != "") {
		cout << "Search ended with an " << error << endl;
	}
	else {
		showTable(res, 0, 0);
	}
}

void View::showTable(PGresult* res, int left, int right, bool range) const {
	std::vector<unsigned long long> width;
	int l = 0;
	int r = PQntuples(res);
	if (range) {
		l = std::max(l, left);
		r = std::min(r, right);
	}
	width.push_back(std::to_string(r).size() + 3);
	for (int j = 0; j < PQnfields(res); j++)
		width.push_back(strlen(PQfname(res, j)) + 3);
	for (int i = l; i < r; i++)
		for (int j = 0; j < PQnfields(res); j++)
			width[j + 1] = std::max(width[j + 1], strlen(PQgetvalue(res, i, j)) + 3);

	cout << std::setw(width[0]) << "#";
	for (int j = 0; j < PQnfields(res); j++)
		cout << std::setw(width[j + 1]) << PQfname(res, j);
	cout << endl;
	for (int i = l; i < r; i++) {

		cout << std::setw(width[0]) << std::to_string(i + 1);
		for (int j = 0; j < PQnfields(res); j++) {
			cout << std::setw(width[j + 1]) << PQgetvalue(res, i, j);

		}
		cout << endl;
	}
}
