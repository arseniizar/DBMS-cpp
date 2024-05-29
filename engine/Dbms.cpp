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
    // not printing if it has an error
    ut_print(Dbms::executor.tmp_t);
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
//            fmt::println("proceeding...");
        }
    }
}


std::pair<std::vector<Column>, Execution_error> Dbms::execute_query(Query &q) {
    Dbms::executor.set_query(q);
    Dbms::executor.execute();
    Execution_result rec = Dbms::executor.get_execution_res();
    if (Dbms::executor.action == Q_action::SELECT) {
        ut_print(Dbms::executor.tmp_cols);
    } else if (Dbms::executor.action == Q_action::DELETE) {
        ut_print(Dbms::executor.tmp_cols);
    } else if (Dbms::executor.action == Q_action::INSERT) {
        ut_print(Dbms::executor.tmp_cols);
    } else if (Dbms::executor.action == Q_action::UPDATE) {
        ut_print(Dbms::executor.tmp_cols);
    } else if (Dbms::executor.action == Q_action::CREATE) {
        auto table_name = Dbms::executor.tmp_t.get_table_name();
        if (is_table_already_exist(table_name)) {
            return executor_error("at EXECUTION: Table with name already exists");
        }
        if (!Dbms::executor.q.get_referenced_table_name().empty()) {
            auto are_relations_ok = Dbms::check_relations();
            if (are_relations_ok) Dbms::populate_keys();
        }
        Dbms::add_table(Dbms::executor.tmp_t);
        if (!Dbms::is_loading) ut_print(Dbms::executor.tmp_t);
    }
    std::string table_name = Dbms::executor.tmp_t.get_table_name();
    Table q_t = *Dbms::find_table_by_name(Dbms::executor.tmp_t.get_table_name());
    q.set_p_table(&q_t);
    auto rec_c = recent_change(&q);
    Dbms::add_rec_change(rec_c);
    Dbms::queries.push_back(q);
    if (!Dbms::executor.get_error().message.empty())
        fmt::println("{}", Dbms::parser.get_error().message);
//    else fmt::println("EXECUTED SUCCESSFULLY");
    return std::make_pair(Dbms::executor.tmp_cols, Execution_error());
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

std::pair<std::vector<Column>, Execution_error> Dbms::executor_error(std::string const &message) {
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

void Dbms::make_save() {
    if (Dbms::tables.empty()) return;
    namespace fs = std::filesystem;
    auto table_txt = std::string();
    auto path = std::string(Dbms::table_saves_path);
    auto tp = std::chrono::system_clock::now();
    auto folder_name = fmt::format("{:%Y_%m_%d %H_%M_%S}", tp);
    std::replace(folder_name.begin(), folder_name.end(), '.', '_');
    auto fs_path = fs::path(path + folder_name);
    if (!fs::exists(fs::path(fs_path))) {
        fs::create_directory(fs::path(path + folder_name));
    }
    for (auto &table: Dbms::tables) {
        std::ofstream ofs(fs_path.string() + R"(\)" + table.get_table_name() + ".txt", std::ofstream::out);
        if (ofs.is_open()) {
            // create table
            ofs << "create table " + table.get_table_name() + "( ";
            for (auto j = 0; j < table.get_columns().size(); ++j) {
                auto col = table.get_columns()[j];
                ofs << col.get_name() + " " + data_types_str[static_cast<int>(col.get_type())];
                if (table.get_primary_key().curr_col_name == col.get_name()) {
                    ofs << " primary key";
                } else if (!table.get_foreign_keys().empty()) {
                    for (auto &fk: table.get_foreign_keys()) {
                        if (fk.curr_col_name == col.get_name())
                            ofs << " foreign key references " + fk.k_a.reference.table_name
                                   + "(" + fk.k_a.reference.column_name + ")";
                    }
                }
                if (j < table.get_columns().size() - 1) ofs << ", ";
            }
            ofs << " )";
            // insert into ... values ...
        }
        ofs.close();
    }
    // do not forget about \n at the end
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


void Dbms::load_save() {
    namespace fs = std::filesystem;
    if (fs::is_empty(fs::path(Dbms::table_saves_path))) return;
    // I need to find most recent dir and then run queries from every txt
    // first line - create
    // second line etc. insert into ... values ... (could be empty)
    auto dir_names = std::vector<std::string>();
    for (auto const &entry: fs::directory_iterator(Dbms::table_saves_path))
        if (fs::is_directory(entry.status()))
            dir_names.push_back(entry.path().filename().string());
    // sorting the date directory names with sort and lambdas
    std::ranges::sort(dir_names,
                      [](std::vector<int> const &vec1, std::vector<int> const &vec2) -> bool {
                          for (auto i = 0; i < vec1.size(); ++i) {
                              if (vec1[i] != vec2[i])
                                  return vec1[i] > vec2[i];
                          }
                          return true;
                      },
                      [](std::string const &str) -> std::vector<int> {
                          auto vec = std::vector<int>();
                          auto match = std::smatch();
                          auto reg = std::regex(R"((\d+)_(\d+)_(\d+) (\d+)_(\d+)_(\d+)_(\d+))");
                          if (std::regex_search(str, match, reg))
                              for (const auto &capture_group: match)
                                  vec.push_back(std::stoi(capture_group.str()));
                          return vec;
                      });
    if (!fs::exists(fs::path(Dbms::table_saves_path + dir_names.front()))
        || fs::is_empty(fs::path(Dbms::table_saves_path + dir_names.front()))) {
        fmt::println("corrupted save with name {}", dir_names.front());
        return;
    }
    for (auto &entry
            : fs::directory_iterator(fs::path(Dbms::table_saves_path + dir_names.front()))) {
        if (entry.is_regular_file()) {
            std::ifstream ifs(entry.path());
            if (!ifs.is_open()) {
                fmt::println("Error while reading {}", entry.path().filename().string());
                return;
            }
            auto command = std::string();
            auto command_number = int(1);
            while (std::getline(ifs, command))
                if (!command.empty() && command != "\n")Dbms::parse_and_execute(command);
            ifs.close();
        }
    }
    // cleaning the loading queries and resetting the flag
    Dbms::is_dbms_changed = false;
    Dbms::queries = std::vector<Query>();
}

void Dbms::print_table_names() {
    auto vec = std::vector<std::string>();
    for (auto &t: Dbms::tables) vec.push_back(t.get_table_name());
    fmt::println("{}", vec);
}

Dbms::Dbms() {
    Dbms::is_dbms_changed = false;
    Dbms::is_loading = true;
    Dbms::load_save();
    Dbms::is_loading = false;
    Dbms::print_table_names();
}

Dbms::~Dbms() {
    if (Dbms::is_dbms_changed) Dbms::make_save();
}
