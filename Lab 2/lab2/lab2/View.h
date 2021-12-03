#pragma once
#include <libpq-fe.h>
#include <memory>
#include <string>
#include "Model.h"
namespace view {
	class View
	{
	public:
		View();
		void printInsert(PGresult* res, const std::string& error) const;
		void printRemove(PGresult* res, const std::string& error) const;
		void printEdit(PGresult* res, const std::string& error) const;
		void printGenerate(PGresult* res, const std::string& error) const;
		void printDrop(PGresult* res, const std::string& error) const;
		void printSearch(PGresult* res, const std::string& error) const;
		void showTable(PGresult* res, int left, int right, bool range = false) const;
	private:

	};
}