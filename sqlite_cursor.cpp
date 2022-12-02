#include "database_handler.h"

namespace zol
{
	namespace sqlite
	{
		sqlite3_cursor::~sqlite3_cursor()
		{
			// do samething
			if(this->statement_){
				delete this->statement_;
			}
		}

		sqlite3_cursor::operator bool() const
		{
			return this->is_rwo_;
		};

		bool sqlite3_cursor::is_null(int column_index)
		{
			return SQLITE_NULL == sqlite3_column_type(this->statement_->get_statement_handle(), column_index);
		}

		sqlite3_prepared_statement *sqlite3_cursor::get_prepared_statment()
		{
			return this->statement_;
		}

		int sqlite3_cursor::int_value(int column_index)
		{
			return !*this ? -1 : [&]
			{
				if (SQLITE_NULL == sqlite3_column_type(this->statement_->get_statement_handle(), column_index))
				{
					return 0;
				}
				else
				{
					return sqlite3_column_int(this->statement_->get_statement_handle(), column_index);
				}
			}();
		}

		double sqlite3_cursor::double_value(int column_index)
		{

			return !*this ? -1 : [&]
			{
				if (SQLITE_NULL == sqlite3_column_type(this->statement_->get_statement_handle(), column_index))
				{
					return (double)0;
				}
				else
				{
					return sqlite3_column_double(this->statement_->get_statement_handle(), column_index);
				}
			}();
		}

		long long int sqlite3_cursor::long_value(int column_index)
		{

			return !*this ? -1 : [&]
			{
				if (SQLITE_NULL == sqlite3_column_type(this->statement_->get_statement_handle(), column_index))
				{
					return (long long int)0;
				}
				else
				{
					return sqlite3_column_int64(this->statement_->get_statement_handle(), column_index);
				}
			}();
		}

		const unsigned char *sqlite3_cursor::string_value(int column_index)
		{

			return !*this ? (const unsigned char *)nullptr : [&]
			{
				if (SQLITE_NULL == sqlite3_column_type(this->statement_->get_statement_handle(), column_index))
				{
					return (const unsigned char *)nullptr;
				}
				else
				{
					return (const unsigned char *)sqlite3_column_text(this->statement_->get_statement_handle(), column_index);
				}
			}();
		}

		std::tuple<const void *, unsigned long long int> sqlite3_cursor::byte_array_value(int column_index)
		{
			return !*this ? std::make_tuple((const void *)nullptr, (int)-1) : [&]
			{
				const void *buf = sqlite3_column_blob(this->statement_->get_statement_handle(), column_index);
				int length = sqlite3_column_bytes(this->statement_->get_statement_handle(), column_index);
				if (buf && length > 0)
				{
					return std::make_tuple(buf, length);
				}
				else
				{
					return std::make_tuple((const void *)nullptr, (int)-1);
				}
			}();
		}
		
		int sqlite3_cursor::get_type_of(int column_index)
		{
			return !*this ? -1 : (int)sqlite3_column_type(this->statement_->get_statement_handle(), column_index);
		}

		bool sqlite3_cursor::next()
		{
			int result = this->statement_->step();
			if (result == -1)
			{
				int repeat_count = 6;
				while (repeat_count-- != 0)
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
					result = this->statement_->step();
					if (result == 0)
						break;
				}
			}
			this->is_rwo_ = (result == 0);
			return this->is_rwo_;
		}

		sqlite3_stmt *sqlite3_cursor::get_statement_handler()
		{
			return this->statement_->get_statement_handle();
		}

		int sqlite3_cursor::get_column_count()
		{
			return sqlite3_column_count(this->statement_->get_statement_handle());
		}

		void sqlite3_cursor::dispose()
		{
			this->statement_->dispose();
		}
	}
} // namespace zol
