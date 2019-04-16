#pragma once
#include <string>
#include <vector>
#include <variant>

namespace DB {

	enum column_t{ INT, CHAR, VARCHAR };

	struct ColumnDef {
		std::string name;
		column_t type;
		int len;
		//default value
		bool canNull;
		//references

	}; // end struct Column

	struct Column {
		std::string name;
		column_t type;
		// value;
		bool isDefault;
	};

	struct createdTable {
		std::string name;
		std::vector<ColumnDef> columns;
		int pk; // -1 indicates no pk, or the index of pk column
	};

	//droppedTable only need name


	enum base_t{ OP, NODE };
	enum op_t{ MATH_OP, LOGICAL_OP, COMPARISON_OP };
	enum node_t{ TABLE_NODE, NUMERIC_NODE, STR_NODE };
	using value_t = std::variant<Column, int, std::string>;	//variant ??  table/numeric/str

	class BaseNode {
		base_t  type;
	};

	class OpNode : public BaseNode {
		op_t type; //override basetype
		BaseNode* left, right;
	};

	class  Node : public BaseNode {
		node_t type; //override basetype
		value_t value;
	};



	enum dmlType{ INSERT, UPDATE, DELETE, SELECT };

	class DmlOp {
		dmlType type;
		BaseNode* whereClause;
	};

	class OneTableOp : public DmlOp {
		std::string tableName;
	};

	class InsertOp : public OneTableOp {
		// several or single
		std::vector<Column> elements;
	};

	class UpdateOp : public OneTableOp {
		
	};

	class DeleteOp : public OneTableOp {

	};

	class SelectOp {

	};
} //end namespace DB