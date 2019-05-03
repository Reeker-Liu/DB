#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include "page.h"

namespace DB::vm { class VM; }

namespace DB::table {
	static vm::VM* vm_;

	using value_t = std::variant<int32_t, std::string>;

	struct TableInfo
	{
		TableInfo(){ }

		TableInfo(std::string tableName,
			std::vector<page::ColumnInfo> columnInfos,
			std::vector<std::string> colNames,
			vm::VM* vm);

		void reset(vm::VM* vm);

		bool hasPK() const;
		page::key_t_t PK_t() const;

		// return NOT_A_PAGE if not fk
		page::page_id_t fk_ref_table_id(uint32_t fk_col);

		// init from outside
		std::string tableName_;
		std::vector<page::ColumnInfo> columnInfos_;
		std::vector<std::string> colNames_;
		const std::unordered_map<page::page_id_t,
			std::unordered_map<int32_t, uint32_t>>*table_pk_ref_INT;
		const std::unordered_map<page::page_id_t,
			std::unordered_map<std::string, uint32_t>>*table_pk_ref_VARCHAR;

		// should init
		//uint32_t pk_col_ = page::TableMetaPage::NOT_A_COLUMN;
		std::vector<std::pair<uint32_t, page::page_id_t>> fks_; // fk col -> table meta page
	};



	struct table_view
	{
		table_view() {}


		std::shared_ptr<const TableInfo> table_info_;
	};

	struct Row
	{

	};

	struct VirtualTable {
		std::vector<Row*> rows;
	};

	//need locks for mutli-threading
	static std::unordered_map<std::string, table::TableInfo> tableBuffer;

	table::TableInfo getTableInfo(const std::string& tableName);

	page::ColumnInfo getColumnInfo(const std::string& tableName, const std::string& columnName);
	
	/*
	bool is_PK(const std::string& tableName, const std::string& col_name);
	bool is_FK(const std::string& tableName, const std::string& col_name);
	bool is_not_null(const std::string& tableName, const std::string& col_name);
	bool is_default_col(const std::string& tableName, const std::string& col_name);
	RetValue get_default_value(const std::string& tableName, const std::string& col_name);
	*/
}