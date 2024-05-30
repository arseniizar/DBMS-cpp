#include "engine/Dbms/Dbms.hpp"

// I will use pl/sql as the basis of sql in this project

auto main() -> int {
    auto db = Dbms();
    db.run();
    return 0;
}