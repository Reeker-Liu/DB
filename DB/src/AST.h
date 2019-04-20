#pragma once
#include <string>
#include <vector>
#include <variant>

namespace DB {

	
	


	namespace VM{
		
		using RetValue = std::variant<bool, int, std::string>;

		bool is_PK(const std::string& tableName, const std::string& col_name);
        bool is_FK(const std::string& tableName, const std::string& col_name);
        bool is_not_null(const std::string& tableName, const std::string& col_name);
        bool is_default_col(const std::string& tableName, const std::string& col_name);
		RetValue get_default_value(const std::string& tableName, const std::string& col_name);

		// onlt used when creating table
        //void insert_column(const std::string&, ColumnInfo*);
		
		struct Row
		{

		};
		RetValue getValue(Row* row, const std::string& tableName, const std::string& columnName) { return 0; }

	}


	namespace AST{
		//===========================================================
		//DDL
		enum class col_t_t { INT, CHAR, VARCHAR };
		struct ColumnInfo {
			std::string columnName;
			col_t_t col_t_;
			unsigned int str_len_;      // used when col_t_ = `CHAR` or `VARCHAR`
			unsigned int constraint_t_;		//constraint_t_t { PK = 1, FK = 2, NOT_NULL = 4, DEFAULT = 8, }
			std::string fkTable;
			std::string defaultStr;
			int defaultInt;
		};
		struct CreateTableInfo
		{
			std::string tableName;
			std::vector<ColumnInfo*> columnInfos;
		};
		using DropTableInfo = std::string;

		//===========================================================
		//DML
		enum class base_t_t{ LOGICAL_OP, COMPARISON_OP, ID, NUMERIC, STR, MATH_OP };
		//enum class atom_t_t {  };
		enum class logical_t_t{ AND, OR };
		enum class comparison_t_t{ EQ, NEQ, LESS, GREATER, LEQ, GEQ, };
		enum class math_t_t{ ADD, SUB, MUL, DIV, MOD, };
		std::string base2str[] = { "LOGICAL_OP", "COMPARISON_OP", "ID", "NUMERIC", "STR", "MATH_OP" };
		std::string logical2str[] = { "AND", "OR" };
		std::string comparison2str[] = { "==", "!=", "<", ">", "<=", ">=" };
		std::string math2str[] = { "+", "-", "*", "/", "%" };

		using RetValue = std::variant<bool, int, std::string>;

		struct BaseExpr{
			BaseExpr(base_t_t base_t):base_t_(base_t){}
			virtual ~BaseExpr() = 0;
			//virtual RetValue getValue(VM::Row*) = 0;

			const base_t_t  base_t_;
		};

		struct NonAtomExpr : public BaseExpr{
			NonAtomExpr(base_t_t base_t):BaseExpr(base_t){}
			virtual ~NonAtomExpr() = 0;
			//virtual RetValue getValue(VM::Row*) = 0;
		};

		struct AtomExpr : public BaseExpr{
			AtomExpr(base_t_t base_t):BaseExpr(base_t){}
			virtual ~AtomExpr() = 0;
			//virtual RetValue getValue(VM::Row*) = 0;
		};

		struct LogicalOpExpr : public NonAtomExpr{
			LogicalOpExpr(logical_t_t logical_t, LogicalOpExpr* left, LogicalOpExpr* right):
				NonAtomExpr(base_t_t::LOGICAL_OP), logical_t_(logical_t), _left(left), _right(right){}
			virtual ~LogicalOpExpr();
			//virtual RetValue getValue(VM::Row*);

			const logical_t_t logical_t_;
			NonAtomExpr* _left;
			NonAtomExpr* _right;
		};

		struct ComparisonOpExpr : public NonAtomExpr{
			//left,right must be AtomExpr*
			ComparisonOpExpr(comparison_t_t comparison_t, AtomExpr* left, AtomExpr* right):
				NonAtomExpr(base_t_t::COMPARISON_OP),  comparison_t_(comparison_t), _left(left), _right(right){}
			virtual ~ComparisonOpExpr();
			//virtual RetValue getValue(VM::Row*);
			
			const comparison_t_t comparison_t_;
			AtomExpr* _left;
			AtomExpr* _right;
		};

		struct MathOpExpr : public AtomExpr{
			MathOpExpr(math_t_t math_t, AtomExpr* left, AtomExpr* right):
				AtomExpr(base_t_t::MATH_OP), math_t_(math_t), _left(left), _right(right){}
			virtual ~MathOpExpr();
			//virtual RetValue getValue(VM::Row*);

			const math_t_t math_t_;
			AtomExpr* _left;
			AtomExpr* _right;
		};

		struct IdExpr : public AtomExpr {
			IdExpr(std::string columnName, std::string tableName = std::string()):
				AtomExpr(base_t_t::ID), _tableName(tableName), _columnName(columnName){}
			virtual ~IdExpr();
			//virtual RetValue getValue(VM::Row*);

			std::string _tableName;
			std::string _columnName;
		};

		struct NumericExpr : public AtomExpr{
			NumericExpr(int value):
				AtomExpr(base_t_t::NUMERIC), _value(value){}
			virtual ~NumericExpr();
			//virtual RetValue getValue(VM::Row*);

			int _value;
		};

		struct StrExpr : public AtomExpr{
			StrExpr(std::string& value):
				AtomExpr(base_t_t::STR), _value(value){}
			virtual ~StrExpr();
			//virtual RetValue getValue();

			std::string _value;
		};


		//for extension, source should be a class, decompose into ExprTableSource、JoinTableSource、SubqueryTableSource
		struct Element{
			std::string name;
			bool isDefault;
			AtomExpr* valueExpr;
		};
		using Elements = std::vector<Element>;
		using Source = std::vector<std::string>;
		struct InsertInfo{
			std::string sourceTable;
			std::vector<Elements> values;
		};

		struct UpdateInfo{
			std::string sourceTable;
			Elements elements;
			BaseExpr* whereExpr;
		};

		struct DeleteInfo{
			std::string sourceTable;
			BaseExpr* whereExpr;
		};

		using OrderbyElement = std::pair<BaseExpr*, bool>;
		struct SelectInfo{
			bool join;
			Source sourceTables;
			Elements elements;
			BaseExpr* whereExpr;
			std::vector<OrderbyElement> orders;	//tuple<column name, is ASC>
		};

		//===========================================================
		//visitors	
		//need reconstruct for atom/id/nonatom
		inline int numericOp(int op1, int op2, math_t_t math_t);
		inline bool comparisonOp(int op1, int op2, comparison_t_t comparison_t);

		//output visitor, output the AST for debug
		class OutputVisitor {
		public:
			OutputVisitor(){ }
			~OutputVisitor(){ }
			void visit(BaseExpr* root);
		private:
			void _visit(BaseExpr* root);
			size_t indent;
		};

		//row visitor, for computing / filtering values of a row
		class RowVisitor {
		public:
			RowVisitor(){ }
			~RowVisitor(){ }
			RetValue visit(BaseExpr* root, VM::Row* row);
		private:
			RetValue _visit(BaseExpr* root);
			VM::Row* _row;
		};

		//atom visitor, for computing math expression without row info
		class AtomVisitor {
		public:
			AtomVisitor() { }
			~AtomVisitor() { }
			RetValue visit(BaseExpr* root);
		private:
			RetValue _visit(BaseExpr* root);
		};

		//===========================================================
		//type for VM
		struct ErrorMsg {
			std::string msg;
		};
		using SQLValue = std::variant < CreateTableInfo, DropTableInfo, SelectInfo, UpdateInfo, InsertInfo, DeleteInfo, ErrorMsg>;
	} //end namespace AST
	
} //end namespace DB
