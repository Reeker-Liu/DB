#pragma once
#include <iostream>
#include "ast.h"
#include "dbexception.h"

namespace DB {
	namespace DML {
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
		RetValue LogicalOpExpr::getValue(VM::Row* row)
		{
			return true;
		}

		ComparisonOpExpr::~ComparisonOpExpr()
		{
			delete _left;
			delete _right;
		}
		RetValue ComparisonOpExpr::getValue(VM::Row* row)
		{
			return true;
		}

		MathOpExpr::~MathOpExpr()
		{
			delete _left;
			delete _right;
		}
		RetValue MathOpExpr::getValue(VM::Row* row)
		{
			return 1;
		}

		IdExpr::~IdExpr()
		{

		}
		RetValue IdExpr::getValue(VM::Row* row)
		{
			return 2;
		}

		NumericExpr::~NumericExpr()
		{

		}
		RetValue NumericExpr::getValue(VM::Row* row)
		{
			return 3;
		}

		StrExpr::~StrExpr()
		{

		}
		RetValue StrExpr::getValue()
		{
			return "";
		}

		//===========================================================
		//visitors

		//output visitor

		void OutputVisitor::visit(BaseExpr* root)
		{
			base_t_t base_t = root->base_t_;
			std::cout << std::string(indent, '-') << base2str[int(base_t)] << "  ";
			++indent;
			switch (base_t)
			{
			case DB::DML::base_t_t::LOGICAL_OP:
				{
					LogicalOpExpr* logicalPtr = static_cast<LogicalOpExpr*>(root);
					std::cout << logical2str[int(logicalPtr->logical_t_)] << std::endl;
					visit(logicalPtr->_left);
					visit(logicalPtr->_right);
				}
				break;
			case DB::DML::base_t_t::COMPARISON_OP:
				{
					ComparisonOpExpr* comparisonPtr = static_cast<ComparisonOpExpr*>(root);
					std::cout << comparison2str[int(comparisonPtr->comparison_t_)] << std::endl;
					visit(comparisonPtr->_left);
					visit(comparisonPtr->_right);
				}
				break;
			case DB::DML::base_t_t::MATH_OP:
				{
					MathOpExpr* mathPtr = static_cast<MathOpExpr*>(root);
					std::cout << math2str[int(mathPtr->math_t_)] << std::endl;
					visit(mathPtr->_left);
					visit(mathPtr->_right);
				}
				break;
			case DB::DML::base_t_t::ID:
				{
					IdExpr* idPtr = static_cast<IdExpr*>(root);
					std::cout << idPtr->_tableName << "." << idPtr->_columnName << std::endl;
				}
				break;
			case DB::DML::base_t_t::NUMERIC:
				{
					NumericExpr* numericPtr = static_cast<NumericExpr*>(root);
					std::cout << numericPtr->_value << std::endl;
				}
				break;
			case DB::DML::base_t_t::STR:
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

		int RowVisitor::numericOp(int op1, int op2, math_t_t math_t)
		{
			switch (math_t)
			{
			case DB::DML::math_t_t::ADD:
				return op1 + op2;
			case DB::DML::math_t_t::SUB:
				return op1 - op2;
			case DB::DML::math_t_t::MUL:
				return op1 * op2;
			case DB::DML::math_t_t::DIV:
				return op1 / op2;
			case DB::DML::math_t_t::MOD:
				return op1 % op2;
			default:
				// no expect to happen
				throw new DB_Exception("wrong math operation type");
			}
		}

		bool RowVisitor::comparisonOp(int op1, int op2, comparison_t_t comparison_t)
		{
			switch (comparison_t)
			{
			case DB::DML::comparison_t_t::EQ:
				return op1 == op2;
			case DB::DML::comparison_t_t::NEQ:
				return op1 != op2;
			case DB::DML::comparison_t_t::LESS:
				return op1 < op2;
			case DB::DML::comparison_t_t::GREATER:
				return op1 > op2;
			case DB::DML::comparison_t_t::LEQ:
				return op1 <= op2;
			case DB::DML::comparison_t_t::GEQ:
				return op1 >= op2;
			default:
				// no expect to happen
				throw new DB_Exception("wrong comparison operation type");
			}
		}

		RetValue RowVisitor::visit(BaseExpr* root)
		{
			base_t_t base_t = root->base_t_;
			switch (base_t)
			{
			case DB::DML::base_t_t::LOGICAL_OP:
				{
					LogicalOpExpr* logicalPtr = static_cast<LogicalOpExpr*>(root);
					RetValue left = visit(logicalPtr->_left);
					RetValue right = visit(logicalPtr->_right);
					std::string op = logical2str[int(logicalPtr->logical_t_)];  //used for exception info

					//RetValue here must be int / str
					auto op1 = std::get_if<bool>(&left);
					auto op2 = std::get_if<bool>(&right);
					if (op1 && op2)
					{	//op1 op2 are both bool
						switch (logicalPtr->logical_t_)
						{
						case DB::DML::logical_t_t::AND:
							return *op1 && *op2;
						case DB::DML::logical_t_t::OR:
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
			case DB::DML::base_t_t::COMPARISON_OP:
				{
					ComparisonOpExpr* comparisonPtr = static_cast<ComparisonOpExpr*>(root);
					RetValue left = visit(comparisonPtr->_left);
					RetValue right = visit(comparisonPtr->_right);
					std::string op = comparison2str[int(comparisonPtr->comparison_t_)];

					//RetValue here must be int / str
					if (auto op1 = std::get_if<std::string>(&left))
					{	//op1 is str
						if (auto op2 = std::get_if<std::string>(&right))
						{	//op2 is also str
							switch (comparisonPtr->comparison_t_)
							{
							case DB::DML::comparison_t_t::EQ:
								return *op1 == *op2;
							case DB::DML::comparison_t_t::NEQ:
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
			case DB::DML::base_t_t::MATH_OP:
				{
					MathOpExpr* mathPtr = static_cast<MathOpExpr*>(root);
					RetValue left = visit(mathPtr->_left);
					RetValue right = visit(mathPtr->_right);
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
					else if(auto op1 = std::get_if<int>(&left))
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
			case DB::DML::base_t_t::ID:
				{
					IdExpr* idPtr = static_cast<IdExpr*>(root);
					//fetch and return value for _tableName._columnName
					return _row->getValue(_row, idPtr->_tableName, idPtr->_columnName);
				}
			case DB::DML::base_t_t::NUMERIC:
				{
					NumericExpr* numericPtr = static_cast<NumericExpr*>(root);
					return numericPtr->_value;
				}
			case DB::DML::base_t_t::STR:
				{
					StrExpr* strPtr = static_cast<StrExpr*>(root);
					return strPtr->_value;
				}
			default:
				// no expect to happen
				throw new DB_Exception("wrong base type");
			}
		}

	} //end namespace DML

} //end namespace DB
