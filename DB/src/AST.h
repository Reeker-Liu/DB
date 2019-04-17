#pragma once
#include <string>
#include <vector>
#include <variant>

namespace DB {

	namespace DDL{
		/*
		*DDL now contains only create/drop table
		*/
		enum class column_t{ INT, CHAR, VARCHAR };

		struct ColumnDef {
			std::string name;
			column_t type;
			int len;
			bool canNull;
		}; // end struct Column

		struct createInfo {
			std::string name;
			std::vector<ColumnDef> columns;
			int pk; // -1 indicates no pk, or the index of pk column
			//default value - DML Expr?
			//references - DML IdExpr?
		};

		//droppedTable only need name
	} //end namespace DDL
	

	namespace DML{
		enum class base_t_t{ ATOM, LOGICAL_OP, COMPARISON_OP,  };
		enum class atom_t_t { ID, NUMERIC, STR, MATH_OP };
		enum class logical_t_t{ AND, OR };
		enum class comparison_t_t{  };
		enum class math_t_t{ };
		// enum numeric_t{ INT };  // extension for more numeric type

		//virtual for all
		struct BaseExpr{
			BaseExpr(base_t_t base_t):base_t_(base_t){}
			virtual ~BaseExpr() = 0;
			virtual void* getOutput() = 0;

			const base_t_t  base_t_;
		};

		struct LogicalOpExpr : public BaseExpr{
			LogicalOpExpr(logical_t_t logical_t, LogicalOpExpr* left, LogicalOpExpr* right):
				BaseExpr(base_t_t::LOGICAL_OP), logical_t_(logical_t), _left(left), _right(right){}
			virtual ~LogicalOpExpr();

			const logical_t_t logical_t_;
			LogicalOpExpr* _left;
			LogicalOpExpr* _right;
		};

		struct ComparisonOpExpr : public BaseExpr{
			//left,right must be AtomExpr*
			ComparisonOpExpr()
			
			const comparison_t_t comparison_t_;
			AtomExpr* left;
			AtomExpr* right;
		};

		struct AtomExpr : public BaseExpr{
			AtomExpr(atom_t_t atom_t):BaseExpr(bast_t_t::ATOM),atom_t_(atom_t){}
			virtual ~AtomExpr() = 0;

			const atom_t_t atom_t_;
		};

		struct MathOpExpr : public AtomExpr{
			

			const math_t_t math_t_;
			AtomExpr* left;
			AtomExpr* right;
		};

		struct IdExpr : public AtomExpr {
			

			std::string tableName;
			std::string columnName;
		};

		struct NumericExpr : public AtomExpr{
			// numeric_t type; // extension for more numeric type
			int value;	// the most precise type of all
		};

		struct StrExpr : public AtomExpr{
			std::string value;
		};


		//for extension, source should be a class, decompose into ExprTableSource、JoinTableSource、SubqueryTableSource
		struct Element{
			std::string name;
			bool isDefault;
			BaseExpr* expr;
		};
		class InsertInfo{
			std::string sourceTable;
			// one insert opreation only insert a single row
			std::vector<Element> elements;
		};

		class UpdateInfo{
			std::string sourceTable;
			std::vector<Element> elements;
			BaseExpr* whereExpr;
		};

		class DeleteInfo{
			std::string sourceTable;
			BaseExpr* whereExpr;
		};

		class SelectInfo{
			bool join;
			std::vector<std::string> sourceTables;
			BaseExpr* whereExpr;
			std::vector<std::tuple<std::string, bool>> orderby;	//tuple<column name, is ASC>
		};

		class BaseVisitor{
			virtual void visit(BaseExpr*) = 0;
		};
		//output visitor
		class OutputVisitor : public BaseVisitor{
			virtual void visit(BaseExpr* root);	//output the AST for debug
		};

		//useful visitors, for each op different?
	} //end namespace DML
	
} //end namespace DB
