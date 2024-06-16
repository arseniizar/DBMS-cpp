#include "../Dbms.hpp"

void Dbms::make_save() {
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
    if (Dbms::tables.empty())  {
        return;
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
            if (!table.are_table_rows_empty()) {
                ofs << "\n";
                // insert into ... values ...
                ofs << "insert into " + table.get_table_name() + " ( ";
                for (auto j = 0; j < table.get_columns().size(); ++j) {
                    auto col = table.get_columns()[j];
                    if (!col.get_rows().empty()) {
                        ofs << col.get_name();
                        if (j < table.get_columns().size() - 1) ofs << ", ";
                    }
                }
                ofs << " ) values ( ";
                auto row_groups = table.get_rows_groups();
                if (!row_groups.empty()) {
                    for (auto i = 0; i < row_groups.size(); ++i) {
                        auto row_group = row_groups[i];
                        for (auto j = 0; j < row_group.size(); ++j) {
                            auto row = row_group[j];
                            ofs << "'" + row.get_data() + "'";
                            if (j < row_group.size() - 1) ofs << ", ";
                        }
                        ofs << " )";
                        if (i < row_groups.size() - 1) ofs << ", (";
                    }
                }
            }
        }
        ofs.close();
    }
}