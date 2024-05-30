#include "../Dbms.hpp"

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