#pragma once

#include <string>
#include <vector>
#include <unordered_map>

namespace DB::table {
	using int8_t = __int8;
	using uint8_t = unsigned __int8;
	using int16_t = __int16;
	using uint16_t = unsigned __int16;
	using int32_t = __int32;
	using uint32_t = unsigned __int32;
	using int64_t = __int64;
	using uint64_t = unsigned __int64;
	using float32 = float;
	using float64 = double;
	static_assert(sizeof(float) == sizeof(int32_t));
	static_assert(sizeof(double) == sizeof(int64_t));

	enum class col_t_t { INT, CHAR, VARCHAR };
	enum constraint_t_t { PK = 1, FK = 2, NOT_NULL = 4, DEFAULT = 8, };
	struct ColumnInfo {
		std::string columnName;
		col_t_t col_t_;
		uint32_t str_len_;      // used when col_t_ = `CHAR` or `VARCHAR`
		uint32_t constraint_t_;
		std::string fkTable;
		std::string defaultStr;
		int defaultInt;
		bool isPK() const noexcept { return constraint_t_ & constraint_t_t::PK; }
		bool isFK() const noexcept { return constraint_t_ & constraint_t_t::FK; }
		bool isNOT_NULL() const noexcept { return constraint_t_ & constraint_t_t::NOT_NULL; }
		bool isDEFAULT() const noexcept { return constraint_t_ & constraint_t_t::DEFAULT; }
		void setPK() { constraint_t_ |= constraint_t_t::PK; }
		void setFK() { constraint_t_ |= constraint_t_t::FK; }
		void setNOT_NULL() { constraint_t_ |= constraint_t_t::NOT_NULL; }
		void setDEFAULT() { constraint_t_ |= constraint_t_t::DEFAULT; }
	};

	using Column = std::pair<std::string, std::string>;	//table column

	struct TableInfo
	{
		std::string tableName;
		std::vector<ColumnInfo> columnInfos;
		uint32_t pk;
		std::vector<uint32_t> fks;
		std::vector<uint32_t> defaults;
		void reset()
		{
			fks.clear();
			defaults.clear();
			for (size_t i = 0, size = columnInfos.size(); i < size; ++i)
			{
				ColumnInfo& column = columnInfos[i];
				if (column.isPK())
					pk = i;
				if (column.isFK())
					fks.push_back(i);
				if (column.isDEFAULT())
					defaults.push_back(i);
			}
		}
	};

	struct Row
	{

	};

	struct VirtualTable {
		std::vector<Row*> rows;
	};


	//need locks for mutli-threading
	static std::unordered_map<std::string, table::TableInfo> tableBuffer;

	table::TableInfo getTableInfo(const std::string& tableName);

	table::ColumnInfo getColumnInfo(const std::string& tableName, const std::string& columnName);
}