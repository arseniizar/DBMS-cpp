#include <bits/stdc++.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "engine/parser.h"

// I will use pl/sql as the basis of sql in this project

auto main() -> int {
    using namespace std;
    auto input = string();
    fmt::println("Write a simple query :)");
//    getline(cin, input);
    input = "SELECT id, name FROM EMP";
    parser parser(input);
    parser.lexeme();
    fmt::println("{}", parser.get_tokens());
    parser.pop();
    parser.pop();
    fmt::println("{}", parser.get_index());
    return 0;
}