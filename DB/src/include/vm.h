#pragma once
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include "table.h"
#include "ast.h"
#include "query.h"


namespace DB::vm {

	ast::RetValue getValue(const table::Row* row, const std::string& tableName, const std::string& columnName);

	std::optional<table::TableInfo> getTableInfo(const std::string& tableName);

	table::VirtualTable scanTable(const std::string& tableName);

	//even isJoin is true, need to check whether the two sources can join
	table::VirtualTable join(table::VirtualTable t1, table::VirtualTable t2, bool pk = false);

	table::VirtualTable projection(table::VirtualTable source, std::vector< ast::AtomExpr*> elements);

	table::VirtualTable sigma(table::VirtualTable source, ast::BaseExpr* whereExpr);

	//bool vm::check_str_len(str, str_t);
	//enum class str_t { ... };

}	//end namespace DB::vm