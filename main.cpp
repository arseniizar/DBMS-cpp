#include "engine/dbms.hpp"

// I will use pl/sql as the basis of sql in this project

auto main() -> int {
    auto db = dbms();
    db.run();
    return 0;
}