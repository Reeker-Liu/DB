#pragma once
#include <string>
#include <iostream>

namespace DB
{
	class DB_Base_Exception
	{
	public:
		void printException() const { this->printEx(); }
		virtual const std::string str() const = 0;
	private:
		virtual void printEx() const = 0;
	};

	class DB_Exception : public DB_Base_Exception
	{
		const std::string _msg;
		virtual void printEx() const { std::cout << (*this) << std::endl; }
	public:
		DB_Exception(std::string msg)
			:_msg(std::move(msg)) {}
		virtual const std::string str() const
		{
			return _msg;
		}
		friend std::ostream& operator<<(std::ostream& os, const DB_Exception& e)
		{
			os << e._msg ;
			return os;
		}
	};

	/*
	 * Universal Exception class.
	 *     display specific error message, line num, and position.
	 */
	class DB_Universal_Exception : public DB_Base_Exception
	{
		const std::string _msg;
		const std::size_t _position;
		virtual void printEx() const { std::cout << (*this) << std::endl; }
	public:
		DB_Universal_Exception(std::string msg, std::size_t position)
			:_msg(std::move(msg)), _position(position) {}
		virtual const std::string str() const
		{
			return _msg + " at position: " + std::to_string(1 + _position);
		}
		friend std::ostream& operator<<(std::ostream& os, const DB_Universal_Exception& e)
		{
			os << e._msg <<  " at position: " << 1 + e._position;
			return os;
		}
	};


	class DB_Runtime_Exception
	{
		const std::string _msg;
		virtual void printEx() const { std::cout << (*this) << std::endl; }
	public:
		DB_Runtime_Exception(std::string msg) :_msg(std::move(msg)) {}
		friend std::ostream& operator<<(std::ostream& os, const DB_Runtime_Exception& e)
		{
			os << "Runtime Error: " << e._msg;
			return os;
		}
	};

}