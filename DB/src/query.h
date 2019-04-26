#pragma once
#include <string>
#include <vector>
#include <variant>
#include <optional>
#include <unordered_map>
#include "table.h"
#include "dbexception.h"
#include "ast.h"

namespace DB::query {
	
	
	//===========================================================
	//DDL
	using CreateTableInfo = table::TableInfo;
	struct DropTableInfo
	{
		std::string tableName;
	};


	//===========================================================
#pragma region DML

	struct Element {
		std::string name;
		bool isDefault;
		ast::AtomExpr* valueExpr;
	};
	using Elements = std::vector<Element>;
	struct InsertInfo {
		std::string sourceTable;
		Elements elements;
	};

	struct UpdateInfo {
		std::string sourceTable;
		Elements elements;
		ast::BaseExpr* whereExpr;
	};

	struct DeleteInfo {
		std::string sourceTable;
		ast::BaseExpr* whereExpr;
	};



	enum class op_t_t { PROJECT, FILTER, JOIN, TABLE };
	struct BaseOp {
		BaseOp(op_t_t op_t) :op_t_(op_t) {}
		virtual ~BaseOp() = 0;
		virtual table::VirtualTable* getOutput() = 0;

		op_t_t op_t_;
	};

	struct ProjectOp : public BaseOp {
		ProjectOp() :BaseOp(op_t_t::PROJECT) {}
		virtual ~ProjectOp() { delete _source; }
		virtual table::VirtualTable* getOutput();

		std::vector<std::string> _names;	//the name of the accordingly element, not sure if useful
		std::vector< ast::AtomExpr*> _elements;	//if empty, $ are used, all columns are needed
		BaseOp* _source;
	};

	struct FilterOp : public BaseOp {
		FilterOp(ast::BaseExpr* whereExpr) :BaseOp(op_t_t::FILTER), _whereExpr(whereExpr) {}
		virtual ~FilterOp() { delete _whereExpr; delete _source; }
		virtual table::VirtualTable* getOutput();

		ast::BaseExpr* _whereExpr;
		BaseOp* _source;
	};

	struct JoinOp : public BaseOp {
		JoinOp() :BaseOp(op_t_t::JOIN) {}
		virtual ~JoinOp() {}
		virtual table::VirtualTable* getOutput();

		std::vector<BaseOp*> _sources;	//currently suppose all sources are TableOp
		bool isJoin;	//even it's true, not sure if the tables can be joined
	};

	struct TableOp : public BaseOp {
		TableOp(const std::string tableName) : BaseOp(op_t_t::TABLE), _tableName(tableName) {}
		virtual ~TableOp(){}
		virtual table::VirtualTable* getOutput();

		std::string _tableName;
	};
	
	
	using OrderbyElement = std::pair<ast::BaseExpr*, bool>;	//	orderExpr, isASC
	struct SelectInfo {
		BaseOp* opRoot;
		std::vector<OrderbyElement> orderbys;
	};

#pragma endregion

	//===========================================================
	//for vm

	struct Exit
	{

	};

	struct ErrorMsg {
		std::string msg;
	};

	//return type to vm, any exception that occurs will be catched and converted into ErrorMsg
	using SQLValue = std::variant < CreateTableInfo, DropTableInfo, SelectInfo, UpdateInfo, InsertInfo, DeleteInfo, Exit, ErrorMsg>;



}	//end namespace DB::query
