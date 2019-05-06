#include "include/table.h"
#include "include/vm.h"

namespace DB::table {

	TableInfo::TableInfo(std::string tableName,
		std::vector<page::ColumnInfo> columnInfos,
		std::vector<std::string> colNames,
		vm::VM* vm)
	{ }

	void TableInfo::reset(vm::VM* vm)
	{ }


	bool TableInfo::hasPK() const
	{
		return true;
	}

	page::key_t_t TableInfo::PK_t() const
	{
		return page::key_t_t::INTEGER;
	}

	// return NOT_A_PAGE if not fk
	page::page_id_t TableInfo::fk_ref_table_id(uint32_t fk_col)
	{
		return 0;
	}

	table::TableInfo getTableInfo(const std::string& tableName)
	{
		if (tableBuffer.find(tableName) == tableBuffer.end())
		{
			if (auto table = table::vm_->getTableInfo(tableName))
			{
				tableBuffer[table->tableName_] = table.value();
			}
			else
			{
				throw std::string("no such table \"" + tableName + "\"");
			}
		}
		return tableBuffer[tableName];
	}

	page::ColumnInfo getColumnInfo(const std::string& tableName, const std::string& columnName)
	{

		table::TableInfo table = getTableInfo(tableName);
		auto& columns = table.columnInfos_;
		auto& columnNames = table.colNames_;
		for (size_t i = 0; i < columns.size(); ++i)
		{
			if (columnNames[i] == columnName)
				return columns[i];
		}
		throw std::string("no such column \"" + columnName + "\" in \"" + tableName + "\"");
	}
}