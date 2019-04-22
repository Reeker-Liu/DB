#pragma once
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include "table.h"

namespace DB::VM {

	using RetValue = std::variant<bool, int, std::string>;

	bool is_PK(const std::string& tableName, const std::string& col_name);
	bool is_FK(const std::string& tableName, const std::string& col_name);
	bool is_not_null(const std::string& tableName, const std::string& col_name);
	bool is_default_col(const std::string& tableName, const std::string& col_name);
	RetValue get_default_value(const std::string& tableName, const std::string& col_name);

	// onlt used when creating table
	//void insert_column(const std::string&, ColumnInfo*);

	struct Row
	{

	};
	RetValue getValue(const Row* row, const std::string& tableName, const std::string& columnName) { return 0; }

	std::optional<TableInfo> getTableInfo(const std::string& tableName) { return TableInfo(); }

	//bool VM::check_str_len(str, str_t);
	//enum class str_t { ... };

}	//end namespace DB::VM