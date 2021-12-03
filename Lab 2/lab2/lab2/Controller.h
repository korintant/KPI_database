#pragma once
#include <map>
#include <string>
#include "libpq-fe.h"
#include "Model.h"
#include "View.h"

namespace controller {
	using std::string;
	using std::map;
	class Controller
	{
	private:
		enum class action : int {
			insert = 1,
			remove,
			edit,
			generate,
			search,
			print,
			drop,
			end
		};

		using action_function = PGresult * (Controller::*)(bool to_print);
		std::map<int, std::pair<string, action_function>> action_map;

	public:
		Controller(std::shared_ptr<model::Model> _model, std::shared_ptr<view::View> _view);
		void interactionLoop();

	private:
		std::string getValue(string& column_name, model::Model::dataTypes type, bool skippable = false);
		std::pair<int, string> chooseTable();
		int chooseRow(std::pair<int, string> table);
		PGresult* performInsert(bool to_print = true);
		PGresult* performRemove(bool to_print = true);
		PGresult* performEdit(bool to_print = true);
		PGresult* performGenerate(bool to_print = true);
		PGresult* performSearch(bool to_print = true);
		PGresult* performPrint(bool to_print = true);
		PGresult* performDrop(bool to_print = true);

		std::shared_ptr<model::Model> model;
		std::shared_ptr<view::View> view;
	};
}

