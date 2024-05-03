#include <bits/stdc++.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "engine/parser.h"

// I will use pl/sql as the basis of sql in this project

auto format_as(query q) {
    return fmt::format("{}", q.to_string());
}

auto main() -> int {
    auto p = parser("SELECT id, name FROM 'b' where a = b");
    query q = p.parse();
    fmt::println("{}", q);
    fmt::println("{}", p.get_error().message);
    return 0;
}