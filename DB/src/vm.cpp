#include "include/vm.h"

namespace DB::vm {

	ast::RetValue VM::getValue(const table::Row* row, const std::string& tableName, const std::string& columnName)
	{ 
		return 0; 
	}

	std::optional<table::TableInfo> VM::getTableInfo(const std::string& tableName)
	{ 
		return table::TableInfo(); 
	}

	table::VirtualTable VM::scanTable(const std::string& tableName)
	{ 
		return table::VirtualTable(); 
	}

	table::VirtualTable VM::join(table::VirtualTable t1, table::VirtualTable t2, bool pk)
	{ 
		return table::VirtualTable(); 
	}

	table::VirtualTable VM::projection(table::VirtualTable source, std::vector< ast::AtomExpr*> elements)
	{ 
		return source; 
	}

	table::VirtualTable VM::sigma(table::VirtualTable source, ast::BaseExpr* whereExpr)
	{
		return source;
	}

}	//end namespace DB::vm