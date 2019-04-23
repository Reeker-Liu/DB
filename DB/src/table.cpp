#include "table.h"
#include"dbexception.h"
#include "VM.h"

namespace DB::table {
	table::TableInfo getTableInfo(const std::string& tableName)
	{
		if (tableBuffer.find(tableName) == tableBuffer.end())
		{
			if (auto table = vm::getTableInfo(tableName))
			{
				tableBuffer[table->tableName] = table.value();
			}
			else
			{
				throw DB_Exception("no such table \"" + tableName + "\"");
			}
		}
		return tableBuffer[tableName];
	}

	table::ColumnInfo getColumnInfo(const std::string& tableName, const std::string& columnName)
	{

		table::TableInfo table = getTableInfo(tableName);
		auto& columns = table.columnInfos;
		for (auto& column : columns)
		{
			if (column.columnName == columnName)
				return column;
		}
		throw DB_Exception("no such column \"" + columnName + "\" in \"" + tableName + "\"");
	}
}