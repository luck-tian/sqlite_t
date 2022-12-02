#ifndef __DATABASE_HANDLER_H__
#define __DATABASE_HANDLER_H__

#include <tuple>
#include <functional>
#include <chrono>
#include <thread>
#include "sqlite3.h"
#include <memory>
#include <string>

namespace zol
{
    namespace sqlite
    {

        class sqlite3_cursor;
        class sqlite3_prepared_statement;
        class sqlite3_database;
        using bind_value = std::function<void(sqlite3_prepared_statement*)>;
        class sqlite3_cursor
        {
        public:
            sqlite3_cursor(sqlite3_prepared_statement* stat) : statement_(stat) {}
            ~sqlite3_cursor();

        public:
            operator bool() const;
            bool is_null(int column_index);
            sqlite3_prepared_statement* get_prepared_statment();
            int int_value(int column_index);
            double double_value(int column_index);
            long long int long_value(int column_index);
            const unsigned char* string_value(int colmun_index);
            std::tuple<const void*, unsigned long long int> byte_array_value(int colmun_index);
            int get_type_of(int colmun_index);
            bool next();
            sqlite3_stmt* get_statement_handler();
            int get_column_count();
            void dispose();

        private:
            sqlite3_prepared_statement* statement_ = nullptr;
            bool is_rwo_ = false;
        }; // class sqlite3_cursor

        class sqlite3_database
        {
        public:
            sqlite3_database(const std::string& file_name);
            ~sqlite3_database();

        public:
            operator bool() const;
            bool table_exists(const std::string& table_name);
            std::shared_ptr<sqlite3_prepared_statement> execute_fast(const std::string& sql);
            int execute_int(const std::string& sql, bind_value);
            void explain_query(std::string& sql, bind_value);
            std::shared_ptr<sqlite3_cursor> query_finalized(const std::string& sql, bind_value);
            void close();
            bool begin_transaction();
            bool commit_transaction();
            sqlite3* get_handle();

        private:
            sqlite3* handle_;
            bool is_open_ = false;
            bool in_transaction_ = false;
        };// class sqlite3_database

        class sqlite3_prepared_statement
        {
        public:
            sqlite3_prepared_statement(sqlite3_database* db, const std::string& sql);
            ~sqlite3_prepared_statement() = default;

        public:
            operator bool() const;
            sqlite3_stmt* get_statement_handle();
            std::shared_ptr<sqlite3_cursor> query(bind_value);
            int step();
            sqlite3_prepared_statement* step_this();
            void reset_query();
            void dispose();
            int bind_integer(int index, int value);
            int bind_double(int index, double value);
            int bind_byte_buffer(int index, const void* value, unsigned long long int len);
            int bind_string(int index, const std::string& value);
            int bind_text(int index, const char*);
            int bind_long(int index, long long int value);
            int bind_null(int index);

        private:
            sqlite3_stmt* handle_ = nullptr;
            bool is_finalized_ = false;

        }; // class sqlite_prepared_statement
    } //namespace zbrowser
} //namespace zol
#endif //__DATABASE_HANDLER_H__