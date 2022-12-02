#include "database_handler.h"
#include <iostream>

namespace zol
{

    namespace sqlite
    {
        sqlite3_prepared_statement::sqlite3_prepared_statement(sqlite3_database *db, const std::string &sql)
        {
            sqlite3_stmt *stmt_handle;
            int result_code = sqlite3_prepare_v2(db->get_handle(), sql.c_str(), -1, &stmt_handle, 0);
            if (SQLITE_OK != result_code)
            {
                std::cout << "sqlite prepared sql fail sql" <<  sql << std::endl;
            }
            else
            {
                std::cout << "sqlite prepared sql success sql" <<  sql << std::endl;
                this->handle_ = stmt_handle;
            }
        }

        sqlite3_prepared_statement::operator bool() const
        {
            return this->is_finalized_;
        }

        sqlite3_stmt *sqlite3_prepared_statement::get_statement_handle()
        {
            return this->handle_;
        }

        std::shared_ptr<sqlite3_cursor> sqlite3_prepared_statement::query(bind_value bind)
        {
            return *this ? nullptr : [&, this]
            {
                int result_code = sqlite3_reset(this->handle_);
                if (SQLITE_OK != result_code)
                    return std::shared_ptr<sqlite3_cursor>(nullptr);
                bind(this);
                return std::make_shared<sqlite3_cursor>(this);
            }();
        }

        // SQLITE_ROW SQLITE_DONE SQLITE_BUSY
        int sqlite3_prepared_statement::step()
        {
            int errcode = sqlite3_step(this->handle_);
            if (errcode == SQLITE_ROW) {
                return 0;
            }
            else if (errcode == SQLITE_DONE) {
                return 1;
            }
            return -1;
        }

        sqlite3_prepared_statement *sqlite3_prepared_statement::step_this()
        {
            sqlite3_step(this->handle_);
            return this;
        }

        void sqlite3_prepared_statement::reset_query()
        {
            int result_code = sqlite3_reset(this->handle_);
            if (SQLITE_OK != result_code)
                std::cout << "sqlite reset fail code " << result_code << std::endl;
        }

        void sqlite3_prepared_statement::dispose()
        {
            if (*this)
                return;
            this->is_finalized_ = true;
            sqlite3_finalize(this->handle_);
        }

        int sqlite3_prepared_statement::bind_integer(int index, int value)
        {
            int result_code = sqlite3_bind_int(this->handle_, index, value);
            if (SQLITE_OK != result_code)
                std::cout << "sqlite bind int fail code " << result_code << std::endl;
            return result_code;
        }

        int sqlite3_prepared_statement::bind_double(int index, double value)
        {
            int result_code = sqlite3_bind_double(this->handle_, index, value);
            if (SQLITE_OK != result_code)
                std::cout << "sqlite bind double fail code " << result_code << std::endl;
            return result_code;
        }
       

        int sqlite3_prepared_statement::bind_byte_buffer(int index, const void *value, unsigned long long int len)
        {
            int result_code = sqlite3_bind_blob(this->handle_, index, value, len, SQLITE_STATIC);
            if (SQLITE_OK != result_code)
                std::cout << "sqlite bind blob fail code " << result_code << std::endl;
            return result_code;
        }

        int sqlite3_prepared_statement::bind_text(int index, const char* value) {
            int result_code = sqlite3_bind_text(this->handle_, index, value, -1, SQLITE_TRANSIENT);
            if (SQLITE_OK != result_code)
                std::cout << "sqlite bind text fail code " << result_code << std::endl;
            return result_code;
        }

        int sqlite3_prepared_statement::bind_string(int index, const std::string &value)
        {
            int result_code = sqlite3_bind_text(this->handle_, index, value.c_str(), -1, SQLITE_TRANSIENT);
            if (SQLITE_OK != result_code)
                std::cout << "sqlite bind text fail code " << result_code << std::endl;
            return result_code;
        }

        int sqlite3_prepared_statement::bind_long(int index, long long int value)
        {
            int result_code = sqlite3_bind_int64(this->handle_, index, value);
            if (SQLITE_OK != result_code)
                std::cout << "sqlite bind long fail code " << result_code << std::endl;
            return result_code;
        }

        int sqlite3_prepared_statement::bind_null(int index)
        {
            int result_code = sqlite3_bind_null(this->handle_, index);
            if (SQLITE_OK != result_code)
                std::cout << "sqlite bind null fail code " << result_code << std::endl;
            return result_code;
        }
    }
} // namespace zol
