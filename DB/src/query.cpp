#pragma once
#include <iostream>
#include "query.h"
#include "dbexception.h"
#include "VM.h"

namespace DB {
	namespace query {
		BaseOp::~BaseOp(){ }

		//===========================================================
		//DML
		table::VirtualTable* ProjectOp::getOutput()
		{
			table::VirtualTable* table = _source->getOutput();
			return vm::projectTable(table, _elements);
		}

		table::VirtualTable* FilterOp::getOutput()
		{
			table::VirtualTable* table = _source->getOutput();
			return vm::filterTable(table, _whereExpr);
		}

		table::VirtualTable* JoinOp::getOutput()
		{
			std::vector<table::VirtualTable*> tables;
			for (auto& source : _sources)
			{
				tables.push_back(source->getOutput());
			}
			return vm::joinTables(tables, this->isJoin);
		}

		table::VirtualTable* TableOp::getOutput()
		{
			if (auto table = vm::getVirtualTable(this->_tableName))
			{
				return table.value();
			}
			else
			{
				throw DB_Exception("no such table \"" + this->_tableName + "\"");
			}
		}

	} //end namespace query

} //end namespace DB
