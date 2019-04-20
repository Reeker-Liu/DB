#pragma once
#include <iostream>
#include "query.h"
#include "dbexception.h"

namespace DB {
	namespace Query {
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
		//visit functions
		int numericOp(int op1, int op2, math_t_t math_t)
		{
			switch (math_t)
			{
			case DB::Query::math_t_t::ADD:
				return op1 + op2;
			case DB::Query::math_t_t::SUB:
				return op1 - op2;
			case DB::Query::math_t_t::MUL:
				return op1 * op2;
			case DB::Query::math_t_t::DIV:
				return op1 / op2;
			case DB::Query::math_t_t::MOD:
				return op1 % op2;
			default:
				// no expect to happen
				throw DB_Exception("wrong math operation type");
			}
		}

		bool comparisonOp(int op1, int op2, comparison_t_t comparison_t)
		{
			switch (comparison_t)
			{
			case DB::Query::comparison_t_t::EQ:
				return op1 == op2;
			case DB::Query::comparison_t_t::NEQ:
				return op1 != op2;
			case DB::Query::comparison_t_t::LESS:
				return op1 < op2;
			case DB::Query::comparison_t_t::GREATER:
				return op1 > op2;
			case DB::Query::comparison_t_t::LEQ:
				return op1 <= op2;
			case DB::Query::comparison_t_t::GEQ:
				return op1 >= op2;
			default:
				// no expect to happen
				throw DB_Exception("wrong comparison operation type");
			}
		}

		//output visit
		void _outputVisit(const BaseExpr* root, std::ostream& os, size_t indent)
		{
			base_t_t base_t = root->base_t_;
			os << std::string(indent * 2, '-') << base2str[int(base_t)] << "  ";
			++indent;
			switch (base_t)
			{
			case DB::Query::base_t_t::LOGICAL_OP:
			{
				const LogicalOpExpr* logicalPtr = static_cast<const LogicalOpExpr*>(root);
				os << logical2str[int(logicalPtr->logical_t_)] << std::endl;
				_outputVisit(logicalPtr->_left, os, indent);
				_outputVisit(logicalPtr->_right, os, indent);
			}
			break;
			case DB::Query::base_t_t::COMPARISON_OP:
			{
				const ComparisonOpExpr* comparisonPtr = static_cast<const ComparisonOpExpr*>(root);
				os << comparison2str[int(comparisonPtr->comparison_t_)] << std::endl;
				_outputVisit(comparisonPtr->_left, os, indent);
				_outputVisit(comparisonPtr->_right, os, indent);
			}
			break;
			case DB::Query::base_t_t::MATH_OP:
			{
				const MathOpExpr* mathPtr = static_cast<const MathOpExpr*>(root);
				os << math2str[int(mathPtr->math_t_)] << std::endl;
				_outputVisit(mathPtr->_left, os, indent);
				_outputVisit(mathPtr->_right, os, indent);
			}
			break;
			case DB::Query::base_t_t::ID:
			{
				const IdExpr* idPtr = static_cast<const IdExpr*>(root);
				os << idPtr->_tableName << "." << idPtr->_columnName << std::endl;
			}
			break;
			case DB::Query::base_t_t::NUMERIC:
			{
				const NumericExpr* numericPtr = static_cast<const NumericExpr*>(root);
				os << numericPtr->_value << std::endl;
			}
			break;
			case DB::Query::base_t_t::STR:
			{
				const StrExpr* strPtr = static_cast<const StrExpr*>(root);
				os << strPtr->_value << std::endl;
			}
			break;
			default:
				os << "Unexpected base_t" << std::endl;
				break;
			}
		}

		void outputVisit(const BaseExpr* root, std::ostream& os)
		{
			_outputVisit(root, os, 0);
		}


		//check visit
		bool _checkVisit(BaseExpr* root) throw (DB_Exception)
		{
			base_t_t base_t = root->base_t_;
			switch (base_t)
			{
			case DB::Query::base_t_t::LOGICAL_OP:
			{
				const LogicalOpExpr* logicalPtr = static_cast<const LogicalOpExpr*>(root);
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
					case DB::Query::logical_t_t::AND:
						return *op1 && *op2;
					case DB::Query::logical_t_t::OR:
						return *op1 || *op2;
					default:
						// no expect to happen
						throw DB_Exception("wrong logical type");
					}
				}
				else
				{	// op1 or op2 is not bool
					throw DB_Exception("operation '" + op + "' only support bool oprands");
				}
			}
			case DB::Query::base_t_t::COMPARISON_OP:
			{
				const ComparisonOpExpr* comparisonPtr = static_cast<const ComparisonOpExpr*>(root);
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
						case DB::Query::comparison_t_t::EQ:
							return *op1 == *op2;
						case DB::Query::comparison_t_t::NEQ:
							return *op1 != *op2;
						default:
							throw DB_Exception("unsupported operation '" + op + "' on strings");
						}
					}
					else
					{	//op is not str
						throw DB_Exception("'" + op + "' on mismatched type");
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
						throw DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else
				{	//op1 is not str/int
					throw DB_Exception("opearion '" + op + "' used on unsupported oprands");
				}
			}
			case DB::Query::base_t_t::MATH_OP:
			case DB::Query::base_t_t::ID:
			case DB::Query::base_t_t::NUMERIC:
			case DB::Query::base_t_t::STR:
			default:
				// no expect to happen
				throw DB_Exception("wrong base type");
			}
		}
		
		bool checkVisit(BaseExpr* root)
		{
			return _checkVisit(root);
		}

		RetValue _checkVisit(AtomExpr* root) throw (DB_Exception)
		{
			base_t_t base_t = root->base_t_;
			switch (base_t)
			{
			case DB::Query::base_t_t::MATH_OP:
			{
				const MathOpExpr* mathPtr = static_cast<const MathOpExpr*>(root);
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
							throw DB_Exception("unsupported operation '" + op + "' on string");
						}
					}
					else
					{	//str op int/bool
						throw DB_Exception("'" + op + "' on mismatched type");
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
						throw DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else
				{	//op1 is not str/int
					throw DB_Exception("opearion '" + op + "' used on unsupported oprands");
				}
			}
			case DB::Query::base_t_t::NUMERIC:
			{
				const NumericExpr* numericPtr = static_cast<const NumericExpr*>(root);
				return numericPtr->_value;
			}
			case DB::Query::base_t_t::STR:
			{
				const StrExpr* strPtr = static_cast<const StrExpr*>(root);
				return strPtr->_value;
			}

			// no expect to happen
			case DB::Query::base_t_t::LOGICAL_OP:
			case DB::Query::base_t_t::COMPARISON_OP:
			case DB::Query::base_t_t::ID:
			default:
				throw DB_Exception("wrong type for atom");
			}
		}

		RetValue checkVisit(AtomExpr* root)
		{
			return _checkVisit(root);
		}



		//vm visit

		
		
		bool _vmVisit(const BaseExpr* root, const VM::Row* row) noexcept
		{
			base_t_t base_t = root->base_t_;
			switch (base_t)
			{
			case DB::Query::base_t_t::LOGICAL_OP:
			{
				const LogicalOpExpr* logicalPtr = static_cast<const LogicalOpExpr*>(root);
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
					case DB::Query::logical_t_t::AND:
						return *op1 && *op2;
					case DB::Query::logical_t_t::OR:
						return *op1 || *op2;
					default:
						// no expect to happen
						throw DB_Exception("wrong logical type");
					}
				}
				else
				{	// op1 or op2 is not bool
					throw DB_Exception("operation '" + op + "' only support bool oprands");
				}
			}
			case DB::Query::base_t_t::COMPARISON_OP:
			{
				const ComparisonOpExpr* comparisonPtr = static_cast<const ComparisonOpExpr*>(root);
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
						case DB::Query::comparison_t_t::EQ:
							return *op1 == *op2;
						case DB::Query::comparison_t_t::NEQ:
							return *op1 != *op2;
						default:
							throw DB_Exception("unsupported operation '" + op + "' on strings");
						}
					}
					else
					{	//op is not str
						throw DB_Exception("'" + op + "' on mismatched type");
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
						throw DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else
				{	//op1 is not str/int
					throw DB_Exception("opearion '" + op + "' used on unsupported oprands");
				}
			}
			case DB::Query::base_t_t::MATH_OP:
			case DB::Query::base_t_t::ID:
			case DB::Query::base_t_t::NUMERIC:
			case DB::Query::base_t_t::STR:
			default:
				// no expect to happen
				throw DB_Exception("wrong base type");
			}
		}

		bool vmVisit(const BaseExpr* root, const VM::Row* row)
		{
			this->_row = row;
			return this->_visit(root);
		}


		RetValue visit(const AtomExpr* root, const VM::Row* row)
		{
			this->_row = row;
			return this->_visit(root);
		}

		RetValue _vmVisit(const AtomExpr* root, const VM::Row* row = nullptr) noexcept
		{
			base_t_t base_t = root->base_t_;
			switch (base_t)
			{
			case DB::Query::base_t_t::MATH_OP:
			{
				const MathOpExpr* mathPtr = static_cast<const MathOpExpr*>(root);
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
							throw DB_Exception("unsupported operation '" + op + "' on string");
						}
					}
					else
					{	//str op int/bool
						throw DB_Exception("'" + op + "' on mismatched type");
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
						throw DB_Exception("'" + op + "' on mismatched type");
					}
				}
				else
				{	//op1 is not str/int
					throw DB_Exception("opearion '" + op + "' used on unsupported oprands");
				}
			}
			case DB::Query::base_t_t::NUMERIC:
			{
				const NumericExpr* numericPtr = static_cast<const NumericExpr*>(root);
				return numericPtr->_value;
			}
			case DB::Query::base_t_t::STR:
			{
				const StrExpr* strPtr = static_cast<const StrExpr*>(root);
				return strPtr->_value;
			}

			// no expect to happen
			case DB::Query::base_t_t::LOGICAL_OP:
			case DB::Query::base_t_t::COMPARISON_OP:
			case DB::Query::base_t_t::ID:
			default:
				throw DB_Exception("wrong type for atom");
			}
		}

	} //end namespace Query

} //end namespace DB
