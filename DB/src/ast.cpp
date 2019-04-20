#pragma once
#include <iostream>
#include "ast.h"
#include "dbexception.h"

namespace DB {
	namespace AST {
		BaseExpr::~BaseExpr()
		{
			
		}

		NonAtomExpr::~NonAtomExpr()
		{

		}

		AtomExpr::~AtomExpr()
		{

		}

		LogicalOpExpr::~LogicalOpExpr()
		{
			delete _left;
			delete _right;
		}

		ComparisonOpExpr::~ComparisonOpExpr()
		{
			delete _left;
			delete _right;
		}

		MathOpExpr::~MathOpExpr()
		{
			delete _left;
			delete _right;
		}

		IdExpr::~IdExpr()
		{

		}

		NumericExpr::~NumericExpr()
		{

		}

		StrExpr::~StrExpr()
		{

		}

		//===========================================================
		//visitors
		int numericOp(int op1, int op2, math_t_t math_t)
		{
			switch (math_t)
			{
			case DB::AST::math_t_t::ADD:
				return op1 + op2;
			case DB::AST::math_t_t::SUB:
				return op1 - op2;
			case DB::AST::math_t_t::MUL:
				return op1 * op2;
			case DB::AST::math_t_t::DIV:
				return op1 / op2;
			case DB::AST::math_t_t::MOD:
				return op1 % op2;
			default:
				// no expect to happen
				throw new DB_Exception("wrong math operation type");
			}
		}

		bool comparisonOp(int op1, int op2, comparison_t_t comparison_t)
		{
			switch (comparison_t)
			{
			case DB::AST::comparison_t_t::EQ:
				return op1 == op2;
			case DB::AST::comparison_t_t::NEQ:
				return op1 != op2;
			case DB::AST::comparison_t_t::LESS:
				return op1 < op2;
			case DB::AST::comparison_t_t::GREATER:
				return op1 > op2;
			case DB::AST::comparison_t_t::LEQ:
				return op1 <= op2;
			case DB::AST::comparison_t_t::GEQ:
				return op1 >= op2;
			default:
				// no expect to happen
				throw new DB_Exception("wrong comparison operation type");
			}
		}

		//output visitor

		void OutputVisitor::visit(BaseExpr* root)
		{
			this->indent = 0;
			this->_visit(root);
		}

		void OutputVisitor::_visit(BaseExpr* root)
		{
			base_t_t base_t = root->base_t_;
			std::cout << std::string(indent, '-') << base2str[int(base_t)] << "  ";
			++indent;
			switch (base_t)
			{
			case DB::AST::base_t_t::LOGICAL_OP:
			{
				LogicalOpExpr* logicalPtr = static_cast<LogicalOpExpr*>(root);
				std::cout << logical2str[int(logicalPtr->logical_t_)] << std::endl;
				visit(logicalPtr->_left);
				visit(logicalPtr->_right);
			}
			break;
			case DB::AST::base_t_t::COMPARISON_OP:
			{
				ComparisonOpExpr* comparisonPtr = static_cast<ComparisonOpExpr*>(root);
				std::cout << comparison2str[int(comparisonPtr->comparison_t_)] << std::endl;
				visit(comparisonPtr->_left);
				visit(comparisonPtr->_right);
			}
			break;
			case DB::AST::base_t_t::MATH_OP:
			{
				MathOpExpr* mathPtr = static_cast<MathOpExpr*>(root);
				std::cout << math2str[int(mathPtr->math_t_)] << std::endl;
				visit(mathPtr->_left);
				visit(mathPtr->_right);
			}
			break;
			case DB::AST::base_t_t::ID:
			{
				IdExpr* idPtr = static_cast<IdExpr*>(root);
				std::cout << idPtr->_tableName << "." << idPtr->_columnName << std::endl;
			}
			break;
			case DB::AST::base_t_t::NUMERIC:
			{
				NumericExpr* numericPtr = static_cast<NumericExpr*>(root);
				std::cout << numericPtr->_value << std::endl;
			}
			break;
			case DB::AST::base_t_t::STR:
			{
				StrExpr* strPtr = static_cast<StrExpr*>(root);
				std::cout << strPtr->_value << std::endl;
			}
			break;
			default:
				std::cout << "Unexpected base_t" << std::endl;
				break;
			}
			--indent;
		}


		//row visitor

		RetValue RowVisitor::visit(BaseExpr* root, VM::Row* row)
		{
			this->_row = row;
			return this->_visit(root);
		}

		RetValue RowVisitor::_visit(BaseExpr* root)
		{
			base_t_t base_t = root->base_t_;
			switch (base_t)
			{
			case DB::AST::base_t_t::LOGICAL_OP:
			{
				LogicalOpExpr* logicalPtr = static_cast<LogicalOpExpr*>(root);
				RetValue left = _visit(logicalPtr->_left);
				RetValue right = _visit(logicalPtr->_right);
				std::string op = logical2str[int(logicalPtr->logical_t_)];  //used for exception info

				//RetValue here must be int / str
				auto op1 = std::get_if<bool>(&left);
				auto op2 = std::get_if<bool>(&right);
				if (op1 && op2)
				{	//op1 op2 are both bool
					switch (logicalPtr->logical_t_)
					{
					case DB::AST::logical_t_t::AND:
						return *op1 && *op2;
					case DB::AST::logical_t_t::OR:
						return *op1 || *op2;
					default:
						// no expect to happen
						throw new DB_Exception("wrong logical type");
					}
				}
				else
				{	// op1 or op2 is not bool
					throw new DB_Exception("operation '" + op + "' only support bool oprands");
				}
			}
			case DB::AST::base_t_t::COMPARISON_OP:
			{
				ComparisonOpExpr* comparisonPtr = static_cast<ComparisonOpExpr*>(root);
				RetValue left = _visit(comparisonPtr->_left);
				RetValue right = _visit(comparisonPtr->_right);
				std::string op = comparison2str[int(comparisonPtr->comparison_t_)];

				//RetValue here must be int / str
				if (auto op1 = std::get_if<std::string>(&left))
				{	//op1 is str
					if (auto op2 = std::get_if<std::string>(&right))
					{	//op2 is also str
						switch (comparisonPtr->comparison_t_)
						{
						case DB::AST::comparison_t_t::EQ:
							return *op1 == *op2;
						case DB::AST::comparison_t_t::NEQ:
							return *op1 != *op2;
						default:
							throw new DB_Exception("unsupported operation '" + op + "' on strings");
						}
					}
					else
					{	//op is not str
						throw new DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else if (auto op1 = std::get_if<int>(&left))
				{	//op1 is int
					if (auto op2 = std::get_if<int>(&right))
					{	//op2 is also int
						return comparisonOp(*op1, *op2, comparisonPtr->comparison_t_);
					}
					else
					{	//int op str/bool
						throw new DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else
				{	//op1 is not str/int
					throw new DB_Exception("opearion '" + op + "' used on unsupported oprands");
				}
			}
			case DB::AST::base_t_t::MATH_OP:
			{
				MathOpExpr* mathPtr = static_cast<MathOpExpr*>(root);
				RetValue left = _visit(mathPtr->_left);
				RetValue right = _visit(mathPtr->_right);
				std::string op = math2str[int(mathPtr->math_t_)];	//used for exception info

				//RetValue here must be int / str
				if (auto op1 = std::get_if<std::string>(&left))
				{	//op1 is str
					if (auto op2 = std::get_if<std::string>(&right))
					{	//op2 is also str
						if (mathPtr->math_t_ == math_t_t::ADD)
						{	//str + str
							return *op1 + *op2;
						}
						else
						{	// str [-*/%] str
							throw new DB_Exception("unsupported operation '" + op + "' on string");
						}
					}
					else
					{	//str op int/bool
						throw new DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else if (auto op1 = std::get_if<int>(&left))
				{	//op1 is int
					if (auto op2 = std::get_if<int>(&right))
					{	//op2 is also int
						return numericOp(*op1, *op2, mathPtr->math_t_);
					}
					else
					{	//int op str/bool
						throw new DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else
				{	//op1 is not str/int
					throw new DB_Exception("opearion '" + op + "' used on unsupported oprands");
				}
			}
			case DB::AST::base_t_t::ID:
			{
				IdExpr* idPtr = static_cast<IdExpr*>(root);
				//fetch and return value for _tableName._columnName
				return VM::getValue(_row, idPtr->_tableName, idPtr->_columnName);
			}
			case DB::AST::base_t_t::NUMERIC:
			{
				NumericExpr* numericPtr = static_cast<NumericExpr*>(root);
				return numericPtr->_value;
			}
			case DB::AST::base_t_t::STR:
			{
				StrExpr* strPtr = static_cast<StrExpr*>(root);
				return strPtr->_value;
			}
			default:
				// no expect to happen
				throw new DB_Exception("wrong base type");
			}
		}


		//atom visitor
		RetValue AtomVisitor::visit(BaseExpr* root)
		{
			return this->_visit(root);
		}

		RetValue AtomVisitor::_visit(BaseExpr* root)
		{
			base_t_t base_t = root->base_t_;
			switch (base_t)
			{
			case DB::AST::base_t_t::MATH_OP:
			{
				MathOpExpr* mathPtr = static_cast<MathOpExpr*>(root);
				RetValue left = _visit(mathPtr->_left);
				RetValue right = _visit(mathPtr->_right);
				std::string op = math2str[int(mathPtr->math_t_)];	//used for exception info

				//RetValue here must be int / str
				if (auto op1 = std::get_if<std::string>(&left))
				{	//op1 is str
					if (auto op2 = std::get_if<std::string>(&right))
					{	//op2 is also str
						if (mathPtr->math_t_ == math_t_t::ADD)
						{	//str + str
							return *op1 + *op2;
						}
						else
						{	// str [-*/%] str
							throw new DB_Exception("unsupported operation '" + op + "' on string");
						}
					}
					else
					{	//str op int/bool
						throw new DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else if (auto op1 = std::get_if<int>(&left))
				{	//op1 is int
					if (auto op2 = std::get_if<int>(&right))
					{	//op2 is also int
						return numericOp(*op1, *op2, mathPtr->math_t_);
					}
					else
					{	//int op str/bool
						throw new DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else
				{	//op1 is not str/int
					throw new DB_Exception("opearion '" + op + "' used on unsupported oprands");
				}
			}
			case DB::AST::base_t_t::NUMERIC:
			{
				NumericExpr* numericPtr = static_cast<NumericExpr*>(root);
				return numericPtr->_value;
			}
			case DB::AST::base_t_t::STR:
			{
				StrExpr* strPtr = static_cast<StrExpr*>(root);
				return strPtr->_value;
			}

			// no expect to happen
			case DB::AST::base_t_t::LOGICAL_OP:
			case DB::AST::base_t_t::COMPARISON_OP:
			case DB::AST::base_t_t::ID:
			default:
				throw new DB_Exception("wrong type for atom visitor");
			}
		}

	} //end namespace AST

} //end namespace DB
