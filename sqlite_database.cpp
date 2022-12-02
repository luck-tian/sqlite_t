#include "database_handler.h"

#ifdef _WIN32
#include <io.h>
#include <direct.h>
#else // _WIN32
#include <sys/stat.h>
#include <dirent.h>
#endif // _WIN32
#include <iostream>
namespace zol
{
    namespace sqlite
    {
        sqlite3_database::sqlite3_database(const std::string &file_name)
        {

             if ((access(file_name.c_str(), 00)) != 0)
            {
                std::string::size_type n;
                std::string::size_type start_search = 0;
                n = file_name.find("/", start_search);
                if(n == 0){
                    n = file_name.find("/", 1);
                }
                while (n != std::string::npos) {
                    std::string path(file_name, 0, n);
#ifdef _WIN32
                    if (_mkdir(path.c_str()) != 0) {
                        std::cout << "file dir create fail " << path.c_str() << std::endl;
                    }
#else // _WIN32
                    if (mkdir(path.c_str(), S_IRUSR | S_IWUSR | S_IXUSR | S_IRWXG | S_IRWXO) != 0 && errno != EEXIST) {
                        std::cout << "file dir create fail " << path.c_str() << std::endl;
                    }
#endif // _WIN32
                    start_search = n + 1;
                    n = file_name.find("/", start_search);
                }
            }
            int result = sqlite3_open(file_name.c_str(), &this->handle_);
            if (SQLITE_OK != result)
            {
                std::cout << "sqlite open db fail code :" << result << std::endl;
            }
            else
            {
                 std::cout << "sqlite open db sucess :" << file_name << std::endl;
                this->is_open_ = true;
            }
        }


        sqlite3_database::operator bool() const
        {
            return this->is_open_;
        }

        sqlite3_database::~sqlite3_database()
        {
            close();
            this->is_open_ = false;
        }

        bool sqlite3_database::table_exists(const std::string &table_name)
        {
            std::string sql("SELECT rowid FROM sqlite_master WHERE type 'table' AND name=?;");
            return execute_int(sql, [&](sqlite3_prepared_statement* statement)
                { statement->bind_string(0, table_name); });
        }

        std::shared_ptr<sqlite3_prepared_statement> sqlite3_database::execute_fast(const std::string &sql)
        {
            return std::make_shared<sqlite3_prepared_statement>(this, sql);
        }

        int sqlite3_database::execute_int(const std::string &sql, bind_value bind)
        {
            return *this ? [&]
            {
                std::shared_ptr<sqlite3_cursor> cursor = query_finalized(sql, bind);
                if (cursor && cursor->next())
                {
                    auto int_value_ = cursor->int_value(0);
                    cursor->dispose();
                    return int_value_;
                }
                else
                {
                    return -1;
                }
            }()
                         : -1;
        }

        void sqlite3_database::explain_query(std::string &sql, bind_value bind)
        {
            if (*this)
            {
                static const char *explain = "EXPLAIN QUERY PLAN ";
                std::shared_ptr<sqlite3_cursor> cursor =
                    sqlite3_prepared_statement(this, sql.insert(0, explain))
                        .query(bind);
                std::string explain_log_info;
                while (cursor->next())
                {
                    int count = cursor->get_column_count();
                    for (; count > 0; count--)
                    {
                        explain_log_info
                            .append(reinterpret_cast<const char *>(cursor->string_value(count)))
                            .append(",");
                    }
                    std::cout << "EXPLAIN QUERY PLAN " << explain_log_info << std::endl;
                }
                cursor->dispose();
            }
        }

        std::shared_ptr<sqlite3_cursor>
            sqlite3_database::query_finalized(const std::string& sql, bind_value bind)
        {
            if(!*this){
                return {};
            }else{
                return (new sqlite3_prepared_statement(this, sql))->query(bind);
            }
        }

        sqlite3 *sqlite3_database::get_handle()
        {
            return this->handle_;
        }

        void sqlite3_database::close()
        {
            if (*this)
            {
                commit_transaction();
                int result = sqlite3_close(this->handle_);
                if (SQLITE_OK != result)
                {
                    std::cout << "sqlite close db fail code" << result << std::endl;
                }
                this->is_open_ = false;
            }
        }

        bool sqlite3_database::begin_transaction()
        {

            return !this->in_transaction_ ? [&]
            {
                this->in_transaction_ = true;
                return !sqlite3_exec(this->handle_, "BEGIN", 0, 0, 0);
            }() : -1;
        }

        bool sqlite3_database::commit_transaction()
        {

            return !this->in_transaction_ ? [&]
            {
                this->in_transaction_ = false;
                return !sqlite3_exec(this->handle_, "COMMIT", 0, 0, 0);
            }() : -1;
        }
    }
} // namespace zol
