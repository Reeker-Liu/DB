#pragma once
#include <string>
#include <vector>

namespace DB {

	namespace VM{
		enum class col_t_t { INT, CHAR, VARCHAR };
		struct constraint_t_t { enum { PK = 1, FK = 2, NOT_NULL = 4, DEFAULT = 8, }; };
		struct ColumnInfo {
			std::string columnName;
			col_t_t col_t_;
			unsigned int str_len_;      // used when col_t_ = `CHAR` or `VARCHAR`
			unsigned int constraint_t_;
			std::string fkTable;
			std::string defaultStr;
			int defaultInt;
		};

		bool is_PK(const std::string& tableName, const std::string& col_name) const;
        bool is_FK(const std::string& tableName, const std::string& col_name) const;
        bool is_not_null(const std::string& tableName, const std::string& col_name) const;
        bool is_default_col(const std::string& tableName, const std::string& col_name) const;
        ValueEntry get_default_value(const std::string& tableName, const std::string& col_name) const;

		// onlt used when creating table
        void insert_column(const std::string&, ColumnInfo*);

	}


	namespace DML{
		enum class base_t_t{ ATOM, LOGICAL_OP, COMPARISON_OP, ID, NUMERIC, STR, MATH_OP };
		//enum class atom_t_t {  };
		enum class logical_t_t{ AND, OR };
		enum class comparison_t_t{ EQ, NEQ, LESS, GREATER, LEQ, GEQ, };
		enum class math_t_t{ ADD, SUB, MUL, DIV, MOD, };

		struct BaseExpr{
			BaseExpr(base_t_t base_t):base_t_(base_t){}
			virtual ~BaseExpr() = 0;
			virtual void* getOutput() = 0;

			const base_t_t  base_t_;
		};

		struct NonAtomExpr : public BaseExpr{
			NonAtomExpr(base_t_t base_t):BaseExpr(base_t){}
			virtual ~NonAtomExpr() = 0;
			virtual void* getOutput() = 0;
		}

		struct LogicalOpExpr : public NonAtomExpr{
			LogicalOpExpr(logical_t_t logical_t, LogicalOpExpr* left, LogicalOpExpr* right):
				NonAtomExpr(base_t_t::LOGICAL_OP), logical_t_(logical_t), _left(left), _right(right){}
			virtual ~LogicalOpExpr();
			virtual void* getOutPut();

			const logical_t_t logical_t_;
			NonAtomExpr* _left;
			NonAtomExpr* _right;
		};

		struct ComparisonOpExpr : public NonAtomExpr{
			//left,right must be AtomExpr*
			ComparisonOpExpr(comparison_t_t comparison_t, AtomExpr* left, AtomExpr* right):
				NonAtomExpr(base_t_t::COMPARISON_OP),  comparison_t_(comparison_t), _left(left), _right(right){}
			virtual ~ComparisonOpExpr();
			virtual void* getOutPut();
			
			const comparison_t_t comparison_t_;
			AtomExpr* _left;
			AtomExpr* _right;
		};

		struct AtomExpr : public BaseExpr{
			AtomExpr(base_t_t base_t):BaseExpr(bast_t){}
			virtual ~AtomExpr() = 0;
			virtual void* getOutPut() = 0;
		};

		struct MathOpExpr : public AtomExpr{
			MathOpExpr(math_t_t math_t, AtomExpr* left, AtomExpr* right):
				AtomExpr(base_t_t::MATH_OP), math_t_(math_t), _left(left), _right(right){}
			virtual ~MathOpExpr();
			virtual void* getOutPut();

			const math_t_t math_t_;
			AtomExpr* _left;
			AtomExpr* _right;
		};

		struct IdExpr : public AtomExpr {
			IdExpr(std::string& columnName, std::string& tableName = ""):
				AtomExpr(base_t_t::ID), _tableName(tableName), _columnName(columnName){}
			virtual ~IdExpr();
			virtual void* getOutPut();

			std::string _tableName;
			std::string _columnName;
		};

		struct NumericExpr : public AtomExpr{
			NumericExpr(int value):
				AtomExpr(base_t_t::NUMERIC), _value(value){}
			virtual ~NumericExpr();

			int _value;
		};

		struct StrExpr : public AtomExpr{
			StrExpr(std::string& value):
				AtomExpr(base_t_t::STR), _value(value){}
			virtual ~StrExpr();

			std::string _value;
		};


		//for extension, source should be a class, decompose into ExprTableSource、JoinTableSource、SubqueryTableSource
		struct Element{
			std::string name;
			bool isDefault;
			AtomExpr* valueExpr;
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
