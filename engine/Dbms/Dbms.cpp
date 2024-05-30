//
// Created by Altezza on 17.05.2024.
//
#include "Dbms.hpp"

void Dbms::run() {
    fmt::println("Welcome to the Arsenii`s database!");
    fmt::println("Here you can to write some simple queries!");
    if (Dbms::tables.empty())
        fmt::println("It looks like your database is empty, create some Table at first");
    fmt::println("If you want to exit, type EXIT :)");
    while (true) {
        auto input = std::string();
        std::getline(std::cin, input);
        if (input == "EXIT") break;
        Dbms::parse_and_execute_with_print(input);
        Dbms::parser.clean_error();
    }
    fmt::println("You have exited the program");
}

void Dbms::parse_and_execute_with_print(std::string const &input) {
    Dbms::parse_and_execute(input);
    // maybe
}

void Dbms::parse_and_execute(const std::string &input) {
    std::pair<Query, Parse_error> parse_pair = Dbms::parse_query(input);
    if (!parse_pair.second.message.empty()) {
        Dbms::parse_errors.push_back(parse_pair.second);
    } else {
        Dbms::is_dbms_changed = true;
        std::pair<std::vector<Column>, Execution_error>
                execution_pair = Dbms::execute_query(parse_pair.first);
        if (!execution_pair.second.message.empty()) {
            Dbms::execution_errors.push_back(execution_pair.second);
        } else {
            fmt::println("proceeding...");
        }
    }
}

std::pair<Query, Parse_error> Dbms::parse_query(std::string const &str) {
    Dbms::parser.input(str);
    Query q = Dbms::parser.parse();
    if (!Dbms::parser.get_error().message.empty())
        fmt::println("{}", Dbms::parser.get_error().message);
//    else fmt::println("SUCCESSFUL QUERY!");
    Dbms::parser.clean();
    return std::make_pair(q, parser.get_error());
}

void Dbms::add_table(Table t) {
    Dbms::table_names.push_back(t.get_table_name());
    Dbms::tables.push_back(t);
}

void Dbms::add_table(std::string const &name, std::vector<Column> const &cols) {
    Dbms::table_names.push_back(name);
    Dbms::tables.emplace_back(name, cols);
}

void Dbms::add_rec_change(const recent_change &rec_c) {
    Dbms::recent_changes.push_back(rec_c);
}

std::pair<std::vector<Column>, Execution_error> Dbms::make_executor_error(std::string const &message) {
    Execution_error err = Execution_error(message);
    Dbms::executor.set_error(err);
    Dbms::executor.set_error(err);
    return std::make_pair(Dbms::executor.tmp_cols, err);
}

// cool data type, just tried to write it like that :)
__gnu_cxx::__normal_iterator<Table *, std::vector<Table>>
Dbms::find_table_by_name(std::string const &name) {
    return std::find_if(Dbms::tables.begin(), Dbms::tables.end(),
                        [&name](Table t) {
                            return t.get_table_name() == name;
                        });
}

bool Dbms::is_table_already_exist(std::string const &table_name) {
    return std::any_of(Dbms::table_names.begin(), table_names.end(),
                       [&table_name](std::string const &str) {
                           return str == table_name;
                       });
}

void Dbms::print_query(Query const &q) {
    ut_print(q);
}

// so in order to make a relation I should check the Column for the
// PRIMARY KEY and FOREIGN KEY or inside the CREATE TABLE
bool Dbms::check_relations() {
    Query q = Dbms::executor.q;
    // fields from another Table
    auto ref_fields = q.get_referenced_fields_names();
    auto is_referenced_table_exists = Dbms::is_table_already_exist
            (q.get_referenced_table_name());
    if (!is_referenced_table_exists) return false;
    auto ref_table = *Dbms::find_table_by_name(q.get_referenced_table_name());
    if (!ref_table.contains_cols_names(ref_fields)) return false;
    return true;
}

void Dbms::populate_keys() {
    // here I will add all the primary and foreign keys to the Table fields
    Query q = Dbms::executor.q;
    auto foreign_keys = q.get_foreign_keys();
    std::for_each(foreign_keys.begin(), foreign_keys.end(),
                  [&q, this](Field const &fk) {
                      Dbms::executor.tmp_t.add_foreign_key(
                              Foreign_key(fk.value, q.get_table_name(), fk.k_a));
                  });
}

std::vector<Query> Dbms::get_create_queries() {
    auto res = std::vector<Query>();
    for (auto &q: Dbms::queries) {
        if (q.get_query_type() == Query_type::Create) res.push_back(q);
    }
    return res;
}

std::vector<Query> Dbms::get_insert_queries() {
    auto res = std::vector<Query>();
    for (auto &q: Dbms::queries) {
        if (q.get_query_type() == Query_type::Insert) res.push_back(q);
    }
    return res;
}


void Dbms::print_table_names() {
    if (Dbms::tables.empty()) return;
    auto vec = std::vector<std::string>();
    for (auto &t: Dbms::tables) vec.push_back(t.get_table_name());
    fmt::println("{}", vec);
}

Dbms::Dbms() {
    Dbms::is_dbms_changed = false;
    Dbms::is_loading = true;
    Dbms::load_save();
    Dbms::is_loading = false;
//    Dbms::print_table_names();
}

Dbms::~Dbms() {
    if (Dbms::is_dbms_changed) Dbms::make_save();
}

void Dbms::add_and_override_cols(const std::string &table_name, std::vector<Column> &cols) {
    auto table = *Dbms::find_table_by_name(table_name);
    auto table_cols = table.get_columns();
    for (auto &t_col: table_cols) {
        for (auto &col: cols) {
            if (col.get_name() == t_col.get_name()) {
                for (auto &row: col.get_rows()) {
                    t_col.add_row(row);
                }
            }
        }
    }
    table.set_columns(table_cols);
    for (auto &t: Dbms::tables)
        if (t.get_table_name() == table_name)
            t = table;
}
