#pragma once
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include "VM.h"
#include "table.h"
#include "dbexception.h"

namespace DB::Query {

	//need locks for mutli-threading
	static std::unordered_map<std::string, TableInfo> tableBuffer;

	TableInfo getTableInfo(const std::string& tableName)
	{
		if (tableBuffer.find(tableName) == tableBuffer.end())
		{
			if (auto table = VM::getTableInfo(tableName))
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

	ColumnInfo getColumnInfo(const std::string& tableName, const std::string& columnName)
	{
		
		TableInfo table = getTableInfo(tableName);
		auto& columns = table.columnInfos;
		for (auto& column : columns)
		{
			if (column.columnName == columnName)
				return column;
		}
		throw DB_Exception("no such column \"" + columnName + "\" in \"" + tableName + "\"");
	}



	//===========================================================
	//DDL
	using CreateTableInfo = TableInfo;
	using DropTableInfo = std::string;


	//===========================================================
#pragma region AST


	enum class base_t_t { LOGICAL_OP, COMPARISON_OP, ID, NUMERIC, STR, MATH_OP };
	//enum class atom_t_t {  };
	enum class logical_t_t { AND, OR };
	enum class comparison_t_t { EQ, NEQ, LESS, GREATER, LEQ, GEQ, };
	enum class math_t_t { ADD, SUB, MUL, DIV, MOD, };
	const std::string base2str[] = { "LOGICAL_OP", "COMPARISON_OP", "ID", "NUMERIC", "STR", "MATH_OP" };
	const std::string logical2str[] = { "AND", "OR" };
	const std::string comparison2str[] = { "==", "!=", "<", ">", "<=", ">=" };
	const std::string math2str[] = { "+", "-", "*", "/", "%" };

	using RetValue = std::variant<bool, int, std::string>;

	struct BaseExpr {
		BaseExpr(base_t_t base_t) :base_t_(base_t) {}
		virtual ~BaseExpr() = 0;
		//virtual RetValue getValue(VM::Row*) = 0;

		const base_t_t  base_t_;
	};

	struct NonAtomExpr : public BaseExpr {
		NonAtomExpr(base_t_t base_t) :BaseExpr(base_t) {}
		virtual ~NonAtomExpr() = 0;
		//virtual RetValue getValue(VM::Row*) = 0;
	};

	struct AtomExpr : public BaseExpr {
		AtomExpr(base_t_t base_t) :BaseExpr(base_t) {}
		virtual ~AtomExpr() = 0;
		//virtual RetValue getValue(VM::Row*) = 0;
	};

	struct LogicalOpExpr : public NonAtomExpr {
		LogicalOpExpr(logical_t_t logical_t, LogicalOpExpr* left, LogicalOpExpr* right) :
			NonAtomExpr(base_t_t::LOGICAL_OP), logical_t_(logical_t), _left(left), _right(right) {}
		virtual ~LogicalOpExpr();
		//virtual RetValue getValue(VM::Row*);

		const logical_t_t logical_t_;
		NonAtomExpr* _left;
		NonAtomExpr* _right;
	};

	struct ComparisonOpExpr : public NonAtomExpr {
		//left,right must be AtomExpr*
		ComparisonOpExpr(comparison_t_t comparison_t, AtomExpr* left, AtomExpr* right) :
			NonAtomExpr(base_t_t::COMPARISON_OP), comparison_t_(comparison_t), _left(left), _right(right) {}
		virtual ~ComparisonOpExpr();
		//virtual RetValue getValue(VM::Row*);

		const comparison_t_t comparison_t_;
		AtomExpr* _left;
		AtomExpr* _right;
	};

	struct MathOpExpr : public AtomExpr {
		MathOpExpr(math_t_t math_t, AtomExpr* left, AtomExpr* right) :
			AtomExpr(base_t_t::MATH_OP), math_t_(math_t), _left(left), _right(right) {}
		virtual ~MathOpExpr();
		//virtual RetValue getValue(VM::Row*);

		const math_t_t math_t_;
		AtomExpr* _left;
		AtomExpr* _right;
	};

	struct IdExpr : public AtomExpr {
		IdExpr(std::string columnName, std::string tableName = std::string()) :
			AtomExpr(base_t_t::ID), _tableName(tableName), _columnName(columnName) {}
		virtual ~IdExpr();
		//virtual RetValue getValue(VM::Row*);

		std::string _tableName;
		std::string _columnName;
	};

	struct NumericExpr : public AtomExpr {
		NumericExpr(int value) :
			AtomExpr(base_t_t::NUMERIC), _value(value) {}
		virtual ~NumericExpr();
		//virtual RetValue getValue(VM::Row*);

		int _value;
	};

	struct StrExpr : public AtomExpr {
		StrExpr(std::string& value) :
			AtomExpr(base_t_t::STR), _value(value) {}
		virtual ~StrExpr();
		//virtual RetValue getValue();

		std::string _value;
	};


	//===========================================================
	//visit functions

	/*
	*output visit, output the ast to the given ostream
	*regardless of validity
	*/
	void outputVisit(const BaseExpr* root, std::ostream& os);


	/*
	*check visit, used in parsing phase
	*	1. check sql semantics,
	*		such as TableA.idd == 123 where there doesn't exist idd column in TableA
	*	2. check the type matching,
	*		such as WHERE "wtf" AND TableA.name == 123,
	*		here string "wtf" is not supported by logicalOp AND,
	*		string TableA.name and number 123 don't matched either
	*
	*for any dismatching, throw DB_Exception
	*if passing, it is guaranteed other visit function will never encounter unexcepted cases
	*
	*the visited expr won't be modified at present,
	*may be modified for optimization in the future
	*/

	//check WHERE clause(expression)
	void checkVisit(const BaseExpr* root, const std::string tableName = "");

	//check others(expressionAtom)
	void checkVisit(const AtomExpr* root, const std::string tableName = "");


	/*
	*vm visit, for VM
	*guarantee no except
	*/
	inline int numericOp(int op1, int op2, math_t_t math_t);
	inline bool comparisonOp(int op1, int op2, comparison_t_t comparison_t);

	//for WHERE clause(expression), used for filtering values of a row
	bool vmVisit(const BaseExpr* root, const VM::Row* row);

	//for others(expressionAtom), used for computing math/string expression and data in the specified row
	RetValue vmVisit(const AtomExpr* root, const VM::Row* row = nullptr);

	


#pragma endregion

	//===========================================================
#pragma region DML

	struct Element {
		std::string name;
		bool isDefault;
		AtomExpr* valueExpr;
	};
	using Elements = std::vector<Element>;
	struct InsertInfo {
		std::string sourceTable;
		Elements elements;
	};

	struct UpdateInfo {
		std::string sourceTable;
		Elements elements;
		BaseExpr* whereExpr;
	};

	struct DeleteInfo {
		std::string sourceTable;
		BaseExpr* whereExpr;
	};

	
	using Source = std::vector<std::string>;
	using OrderbyElement = std::pair<BaseExpr*, bool>;	//	orderExpr, isASC
	struct SelectInfo {
		bool join;
		Source sourceTables;
		Elements elements;	//if empty, $ are used, all columns are needed
		BaseExpr* whereExpr;		
		std::vector<OrderbyElement> orderbys;
	};

#pragma endregion

	//===========================================================
	//type for VM

	struct ErrorMsg {
		std::string msg;
	};

	//return type to VM, any exception that occurs will be catched and converted into ErrorMsg
	using SQLValue = std::variant < CreateTableInfo, DropTableInfo, SelectInfo, UpdateInfo, InsertInfo, DeleteInfo, ErrorMsg>;


}	//end namespace DB::Query
