#pragma once

#include <string>

namespace DB::page
{
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

	using page_id_t = uint32_t;

	enum class col_t_t { INTEGER, CHAR, VARCHAR };
	using key_t_t = col_t_t;
	enum constraint_t_t { PK = 1, FK = 2, NOT_NULL = 4, DEFAULT = 8, };
	struct ColumnInfo {
		uint32_t col_name_offset_;
		col_t_t col_t_;
		uint16_t str_len_ = 0;                  // used when col_t_ = `CHAR` or `VARCHAR`
		uint16_t constraint_t_ = 0;
		uint32_t other_value_;					// table_id     if constraint_t_ = `FK`
												// value_offset if constraint_t_ = `DEFAULT`

		uint32_t vEntry_offset = 0;             // denotes the offset at ValueEntry
												// do not flush to disk

		bool isPK() const noexcept { return constraint_t_ & constraint_t_t::PK; }
		bool isFK() const noexcept { return constraint_t_ & constraint_t_t::FK; }
		bool isNOT_NULL() const noexcept { return constraint_t_ & constraint_t_t::NOT_NULL; }
		bool isDEFAULT() const noexcept { return constraint_t_ & constraint_t_t::DEFAULT; }
		void setPK() { constraint_t_ |= constraint_t_t::PK; }
		void setFK() { constraint_t_ |= constraint_t_t::FK; }
		void setNOT_NULL() { constraint_t_ |= constraint_t_t::NOT_NULL; }
		void setDEFAULT() { constraint_t_ |= constraint_t_t::DEFAULT; }
	};
}