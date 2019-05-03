#include "include/vm.h"

namespace DB::vm {

	ast::RetValue getValue(const table::Row* row, const std::string& tableName, const std::string& columnName)
	{ 
		return 0; 
	}

	std::optional<table::TableInfo> getTableInfo(const std::string& tableName) 
	{ 
		return table::TableInfo(); 
	}

	table::VirtualTable scanTable(const std::string& tableName) 
	{ 
		return table::VirtualTable(); 
	}

	table::VirtualTable join(table::VirtualTable t1, table::VirtualTable t2, bool pk)
	{ 
		return table::VirtualTable(); 
	}

	table::VirtualTable projection(table::VirtualTable source, std::vector< ast::AtomExpr*> elements) 
	{ 
		return source; 
	}

	table::VirtualTable sigma(table::VirtualTable source, ast::BaseExpr* whereExpr)
	{
		return source;
	}

}	//end namespace DB::vm