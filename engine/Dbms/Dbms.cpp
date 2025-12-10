#include "Dbms.hpp"

#include <iostream>
#include <algorithm>
#include <fstream>
#include <ranges>
#include "../utils/ut_contains_elems/ut_contains_elems.hpp"
#include <fmt/core.h>

#include "engine/utils/ut_print/ut_print.hpp"

void Dbms::load_prompt() {
    while (true) {
        fmt::println("What save do you want to load? (type corresponding number)");
        fmt::println("1 - load newest");
        fmt::println("2 - I want to load a particular save");
        fmt::println("3 - load empty DBMS");
        fmt::println("4 - exit loading menu");
        auto input = std::string();
        std::getline(std::cin, input);
        if (input != "1" and input != "2" and input != "3" and input != "4") {
            fmt::println("You need to write 1,2 or 3 or DONE in order to proceed");
            continue;
        }

        if (input == "1") {
            Dbms::load_save();
        }
        else if (input == "2") {
            Dbms::load_save_menu();
        }
        else if (input == "3") {
            fmt::println("You have loaded an empty DBMS");
        }
        else if (input == "4") {
            fmt::println("You have exited DBMS loading menu");
            break;
        }
        else {
            fmt::println("You should type an appropriate number");
        }
    }
}

std::pair<Query, Parse_error> Dbms::parse_query(std::string const& str) {
    parser.input(str);
    Query q = parser.parse();
    return std::make_pair(q, parser.get_error());
}

void Dbms::add_table(Table t) {
    Dbms::table_names.push_back(t.get_table_name());
    Dbms::tables.push_back(t);
}

void Dbms::add_table(std::string const& name, std::vector<Column> const& cols) {
    Dbms::table_names.push_back(name);
    Dbms::tables.emplace_back(name, cols);
}

void Dbms::add_rec_change(const recent_change& rec_c) {
    Dbms::recent_changes.push_back(rec_c);
}

std::pair<std::vector<Column>, Execution_error> Dbms::make_executor_error(std::string const& message) {
    Execution_error err = Execution_error(message);
    Dbms::executor.set_error(err);
    Dbms::executor.set_error(err);
    return std::make_pair(Dbms::executor.tmp_cols, err);
}

Table Dbms::find_table_by_name(std::string const& name) {
    return *std::find_if(Dbms::tables.begin(), Dbms::tables.end(),
                         [&name](Table t) {
                             return t.get_table_name() == name;
                         });
}

bool Dbms::is_table_already_exist(std::string const& table_name) {
    return std::any_of(Dbms::table_names.begin(), table_names.end(),
                       [&table_name](std::string const& str) {
                           return str == table_name;
                       });
}

// so in order to make a relation I should check the Column for the
// PRIMARY KEY and FOREIGN KEY or inside the CREATE TABLE
bool Dbms::check_relations() {
    Query q = Dbms::executor.q;
    // fields from another Table
    auto ref_fields = q.get_referenced_fields_names();
    auto is_referenced_table_exists = Dbms::is_table_already_exist(q.get_referenced_table_name());
    if (!is_referenced_table_exists)
        return false;
    auto ref_table = Dbms::find_table_by_name(q.get_referenced_table_name());
    if (!ref_table.contains_cols_names(ref_fields))
        return false;
    return true;
}

void Dbms::populate_keys() {
    // here I will add all the primary and foreign keys to the Table fields
    Query q = Dbms::executor.q;
    auto foreign_keys = q.get_foreign_keys();
    std::for_each(foreign_keys.begin(), foreign_keys.end(),
                  [&q, this](Field const& fk) {
                      Dbms::executor.tmp_t.add_foreign_key(
                          Foreign_key(fk.value, q.get_table_name(), fk.k_a));
                  });
}

std::vector<Query> Dbms::get_create_queries() {
    auto res = std::vector<Query>();
    for (auto& q : Dbms::queries) {
        if (q.get_query_type() == Query_type::Create)
            res.push_back(q);
    }
    return res;
}

std::vector<Query> Dbms::get_insert_queries() {
    auto res = std::vector<Query>();
    for (auto& q : Dbms::queries) {
        if (q.get_query_type() == Query_type::Insert)
            res.push_back(q);
    }
    return res;
}

void Dbms::print_table_names() {
    if (Dbms::tables.empty()) {
        fmt::println("[ *empty* ]");
        return;
    }
    auto vec = std::vector<std::string>();
    for (auto& t : Dbms::tables)
        vec.push_back(t.get_table_name());
    fmt::println("{}", vec);
}

Dbms::Dbms() {
    Dbms::is_dbms_changed = false;
    Dbms::is_loading = true;

    try {
        if (!std::filesystem::exists(table_saves_path)) {
            std::filesystem::create_directories(table_saves_path);
            fmt::println("Created saves directory at: {}", table_saves_path);
        }
    }
    catch (const std::filesystem::filesystem_error& e) {
        fmt::println("Error creating directory: {}", e.what());
    }
}

Dbms::~Dbms() {
    if (Dbms::is_dbms_changed)
        Dbms::make_save();
}

void Dbms::create_default_database() {
    fmt::println("No saves found. Creating a default database...");

    process_query("CREATE TABLE employees (id INTEGER, name NVARCHAR2, department NVARCHAR2, salary INTEGER)");
    process_query("CREATE TABLE departments (id INTEGER, name NVARCHAR2, location NVARCHAR2)");

    process_query(
        "INSERT INTO departments (id, name, location) VALUES ('1', 'Engineering', 'Kyiv'), ('2', 'Human Resources', 'Lviv'), ('3', 'Sales', 'Remote')");

    process_query(
        "INSERT INTO employees (id, name, department, salary) VALUES ('101', 'Arsenii Zarudniuk', 'Engineering', '60000')");
    process_query(
        "INSERT INTO employees (id, name, department, salary) VALUES ('102', 'Olena Ivanova', 'Human Resources', '45000')");
    process_query(
        "INSERT INTO employees (id, name, department, salary) VALUES ('103', 'Max Popov', 'Engineering', '55000')");
    process_query(
        "INSERT INTO employees (id, name, department, salary) VALUES ('104', 'Yana Koval', 'Sales', '50000')");
    process_query(
        "INSERT INTO employees (id, name, department, salary) VALUES ('105', 'Petro Bilyk', 'Sales', '48000')");

    is_dbms_changed = true;
}

Execution_error Dbms::add_and_override_cols(const std::string& table_name, std::vector<Column> cols) {
    auto table = Dbms::find_table_by_name(table_name);
    // delete from with an empty where
    if (Dbms::executor.q.get_query_type() == Query_type::Delete and Table("Func", cols).are_table_rows_empty()) {
        auto table_cols = table.get_columns();
        for (auto& col : table_cols) {
            // erase all rows
            col.set_rows(std::vector<Row>());
        }
        table.set_columns(table_cols);
        for (auto& t : Dbms::tables)
            if (t.get_table_name() == table_name)
                t = table;
        return {};
    }
    auto table_cols = table.get_columns();
    const bool are_cols_same = ut_contains_elems(
        table.get_columns_names(),
        Table("func", cols).get_columns_names());
    if (!are_cols_same) {
        const std::string message = "at EXECUTION: unable to override nonexistent columns";
        Dbms::executor.error = Execution_error(message);
        return Execution_error(message);
    }
    for (auto& t_col : table_cols) {
        for (auto& col : cols) {
            if (col.get_name() == t_col.get_name()) {
                for (auto& row : col.get_rows()) {
                    if (const Data_type row_type = return_data_type(row.get_data()); row_type != t_col.get_type()) {
                        const std::string message = "at EXECUTION: unable to override "
                            "columns with improper data type";
                        Dbms::executor.error = Execution_error(message);
                        return Execution_error(message);
                    }
                    t_col.add_row(row);
                }
            }
        }
    }
    table.set_columns(table_cols);
    for (auto& t : Dbms::tables)
        if (t.get_table_name() == table_name)
            t = table;
    return {};
}

std::string Dbms::process_query_to_string(const std::string& input) {
    std::string lower_input = input;
    std::ranges::transform(lower_input, lower_input.begin(),
                           [](unsigned char c) { return std::tolower(c); });
    if (lower_input == "-tables") {
        std::stringstream ss;
        if (tables.empty()) { ss << "[ *empty* ]"; }
        else { for (auto& t : tables) { ss << t.get_table_name() << "\n"; } }
        return ss.str();
    }
    if (lower_input == "-help") {
        std::string helpText =
            "Available commands with examples:\n\n"
            "1. CREATE TABLE\n"
            "   CREATE TABLE users (id INTEGER, name NVARCHAR2, is_active INTEGER);\n\n"
            "2. DROP TABLE\n"
            "   DROP TABLE users;\n\n"
            "3. INSERT INTO\n"
            "   INSERT INTO users (id, name) VALUES ('1', 'Arsenii'), ('2', 'Olena');\n\n"
            "4. SELECT\n"
            "   SELECT id, name FROM users WHERE is_active = '1';\n"
            "   SELECT COUNT(*) FROM users GROUP BY city HAVING COUNT(*) > 5;\n\n"
            "5. UPDATE\n"
            "   UPDATE users SET name = 'Yana' WHERE id = '2';\n\n"
            "6. DELETE FROM\n"
            "   DELETE FROM users WHERE id = '3';\n";
        return helpText;
    }
    if (lower_input == "-load") {
        load_prompt();
        return "Load prompt executed";
    }
    if (lower_input == "exit") {
        return "exit";
    }

    parser.clean();
    auto [query, parser_error] = parse_query(input);

    if (!parser_error.message.empty()) {
        return "Parse error: " + parser_error.message;
    }

    is_dbms_changed = true;
    auto [columns, execution_error] = execute_query(query);


    if (!execution_error.message.empty()) {
        return "Execution error: " + execution_error.message;
    }

    if (query.get_query_type() == Query_type::Select && !columns.empty()) {
        std::stringstream buffer;
        auto old_cout_buf = std::cout.rdbuf(buffer.rdbuf());
        ut_print(columns);
        std::cout.rdbuf(old_cout_buf);
        return buffer.str();
    }

    return "Query executed successfully";
}


QueryResult Dbms::process_query(const std::string& input) {
    std::string lower_input = input;
    std::ranges::transform(lower_input, lower_input.begin(),
                           [](unsigned char c) { return std::tolower(c); });

    if (lower_input == "-tables") {
        std::stringstream ss;
        if (tables.empty()) {
            ss << "[ *empty* ]";
        }
        else {
            for (auto& t : tables) {
                ss << t.get_table_name() << "\n";
            }
        }
        return ss.str();
    }
    if (lower_input == "-help") {
        std::string helpText =
            "Available commands with examples:\n\n"
            "1. CREATE TABLE\n"
            "   CREATE TABLE users (id INTEGER, name NVARCHAR2, is_active INTEGER);\n\n"
            "2. DROP TABLE\n"
            "   DROP TABLE users;\n\n"
            "3. INSERT INTO\n"
            "   INSERT INTO users (id, name) VALUES ('1', 'Arsenii'), ('2', 'Olena');\n\n"
            "4. SELECT\n"
            "   SELECT id, name FROM users WHERE is_active = '1';\n"
            "   SELECT COUNT(*) FROM users GROUP BY city HAVING COUNT(*) > 5;\n\n"
            "5. UPDATE\n"
            "   UPDATE users SET name = 'Yana' WHERE id = '2';\n\n"
            "6. DELETE FROM\n"
            "   DELETE FROM users WHERE id = '3';\n";
        return helpText;
    }
    if (lower_input == "-load") {
        load_prompt();
        return std::string("Load prompt executed in console.");
    }

    parser.clean();
    auto [query, parser_error] = parse_query(input);

    if (!parser_error.message.empty()) {
        return "Parse error: " + parser_error.message;
    }

    is_dbms_changed = true;
    auto [columns, execution_error] = execute_query(query);

    if (!execution_error.message.empty()) {
        return "Execution error: " + execution_error.message;
    }

    if (query.get_query_type() == Query_type::Select) {
        return columns;
    }

    return std::string("Query executed successfully");
}

bool Dbms::load_database_from_path(const std::string& path) {
    namespace fs = std::filesystem;

    tables.clear();
    table_names.clear();
    queries.clear();

    const fs::path db_path(path);
    if (!fs::exists(db_path) || !fs::is_directory(db_path)) {
        fmt::println("Error: Provided path is not a valid directory.");
        return false;
    }

    is_loading = true;
    for (auto const& entry : fs::directory_iterator(db_path)) {
        if (entry.is_regular_file() && entry.path().extension() == ".txt") {
            std::ifstream ifs(entry.path());
            if (!ifs.is_open()) {
                fmt::println("Error: Could not open file {}", entry.path().string());
                continue;
            }
            std::string command;
            while (std::getline(ifs, command)) {
                if (!command.empty()) {
                    process_query(command);
                }
            }
        }
    }
    is_loading = false;
    is_dbms_changed = false;

    fmt::println("Database loaded successfully from {}", path);
    return true;
}

std::vector<std::string> Dbms::get_table_names() const {
    std::vector<std::string> names;
    names.reserve(tables.size());

    for (const auto& table : tables) {
        names.push_back(table.get_table_name());
    }
    return names;
}
