#pragma once
#include <string>
#include <vector>
#include <optional>
#include <variant>
#include "table.h"
#include "ast.h"


namespace DB::vm {

	using RetValue = std::variant<bool, int, std::string>;

	bool is_PK(const std::string& tableName, const std::string& col_name);
	bool is_FK(const std::string& tableName, const std::string& col_name);
	bool is_not_null(const std::string& tableName, const std::string& col_name);
	bool is_default_col(const std::string& tableName, const std::string& col_name);
	RetValue get_default_value(const std::string& tableName, const std::string& col_name);

	// onlt used when creating table
	//void insert_column(const std::string&, ColumnInfo*);

	
	static RetValue getValue(const table::Row* row, const std::string& tableName, const std::string& columnName) { return 0; }

	static std::optional<table::TableInfo> getTableInfo(const std::string& tableName) { return table::TableInfo(); }

	static std::optional<table::VirtualTable*> getVirtualTable(const std::string& tableName) { return new table::VirtualTable(); }

	//even isJoin is true, need to check whether the two sources can join
	static table::VirtualTable* joinTables(std::vector<table::VirtualTable*> sources, bool isJoin = false){ return new table::VirtualTable(); }

	static table::VirtualTable* projectTable(table::VirtualTable* source, std::vector< ast::AtomExpr*> elements){ return source; }

	//this is a simple init implementation 
	static table::VirtualTable* filterTable(table::VirtualTable* source, ast::BaseExpr* whereExpr)
	{
		return source;
	}

	//bool vm::check_str_len(str, str_t);
	//enum class str_t { ... };

}	//end namespace DB::vm