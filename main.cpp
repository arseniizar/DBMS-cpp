#include <bits/stdc++.h>
#include <fmt/core.h>
#include <fmt/ranges.h>
#include "engine/dbms.h"

// I will use pl/sql as the basis of sql in this project

auto format_as(query q) {
    return fmt::format("{}", q.to_string());
}

auto main() -> int {
    auto db = dbms();
    db.parser.input("SELECT * FROM 'b' where a = b");
    query q = db.parser.parse();
    fmt::println("{}", q);
    fmt::println("{}", db.parser.get_error().message);
    return 0;
}