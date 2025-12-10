#include "../Dbms.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <string>
#include <regex>

void Dbms::load_save() {
    namespace fs = std::filesystem;
    if (fs::is_empty(fs::path(Dbms::table_saves_path))) {
        fmt::println("There are no saves available");
        return;
    };

    auto dir_names = std::vector<std::string>();
    for (auto const &entry: fs::directory_iterator(Dbms::table_saves_path))
        if (fs::is_directory(entry.status()))
            dir_names.push_back(entry.path().filename().string());

    std::ranges::sort(dir_names, [](const std::string& a, const std::string& b) {
        return a > b;
    });

    if (dir_names.empty()) return;

    auto newest_save_path = fs::path(Dbms::table_saves_path) / dir_names.front();

    if (!fs::exists(newest_save_path) || fs::is_empty(newest_save_path)) {
        fmt::println("corrupted save with name {}", newest_save_path.filename().string());
        return;
    }

    for (auto const& entry : fs::directory_iterator(newest_save_path)) {
        if (entry.is_regular_file()) {
            std::ifstream ifs(entry.path());
            if (!ifs.is_open()) {
                fmt::println("Error while reading {}", entry.path().filename().string());
                continue;
            }
            std::string command;
            while (std::getline(ifs, command)) {
                if (!command.empty() && command != "\n") {
                    process_query(command);
                }
            }
            ifs.close();
        }
    }

    Dbms::queries.clear();
    Dbms::is_dbms_changed = false;
    Dbms::is_loading = false;
}

void Dbms::load_save_menu() {
    namespace fs = std::filesystem;
    if (fs::is_empty(fs::path(Dbms::table_saves_path))) {
        fmt::println("There are no saves available");
        return;
    }

    auto dir_names = std::vector<std::string>();
    for (auto const &entry: fs::directory_iterator(Dbms::table_saves_path))
        if (fs::is_directory(entry.status()))
            dir_names.push_back(entry.path().filename().string());

    std::ranges::sort(dir_names, [](const std::string& a, const std::string& b) {
        return a > b;
    });

    if (dir_names.empty()) return;

    fmt::println("choose the save folder (name of the folder is the save date):");
    fmt::println("-------------------------------------------------------------------");
    for (size_t i = 0; i < dir_names.size(); ++i) {
        fmt::println("{} - {}", i, dir_names.at(i));
    }
    fmt::println("-------------------------------------------------------------------");

    std::string input;
    size_t index;
    while (true) {
        std::getline(std::cin, input);
        try {
            index = std::stoul(input);
            if (index < dir_names.size()) {
                break;
            }
            fmt::println("Error: Index out of range. Please type an appropriate number.");
        } catch (const std::exception&) {
            fmt::println("Error: Invalid input. Please type a number.");
        }
    }

    auto selected_dir = dir_names.at(index);
    auto selected_path = fs::path(Dbms::table_saves_path) / selected_dir;

    if (!fs::exists(selected_path) || fs::is_empty(selected_path)) {
        fmt::println("corrupted or empty save with name {}", selected_dir);
        return;
    }

    for (auto const& entry : fs::directory_iterator(selected_path)) {
        if (entry.is_regular_file()) {
            std::ifstream ifs(entry.path());
            if (!ifs.is_open()) {
                fmt::println("Error while reading {}", entry.path().filename().string());
                continue;
            }
            std::string command;
            while (std::getline(ifs, command)) {
                if (!command.empty() && command != "\n") {
                    process_query(command);
                }
            }
            ifs.close();
        }
    }

    Dbms::queries.clear();
    Dbms::is_dbms_changed = false;
    Dbms::is_loading = false;
}
