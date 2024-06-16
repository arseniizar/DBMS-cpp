#include "../Parser.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void Parser::step_type() {
    auto peeked = Parser::peek();
    str_toupper(peeked);
    if (peeked == "SELECT") {
        Parser::q.set_query_type(Query_type::Select);
        Parser::pop();
        Parser::step = Step::select_field;
    } else if (peeked == "INSERT INTO") {
        Parser::q.set_query_type(Query_type::Insert);
        Parser::pop();
        Parser::step = Step::insert_table;
//    } else if (peeked == "UPDATE") {
//        Parser::q.set_query_type(Query_type::Update);
//        Parser::pop();
//        Parser::step = Step::update_table;
    } else if (peeked == "DELETE FROM") {
        Parser::q.set_query_type(Query_type::Delete);
        Parser::pop();
        Parser::step = Step::delete_from_table;
    } else if (peeked == "CREATE TABLE") {
        Parser::q.set_query_type(Query_type::Create);
        Parser::pop();
        Parser::step = Step::create_table;
    } else if (peeked == "DROP TABLE") {
        Parser::q.set_query_type(Query_type::Drop);
        Parser::pop();
        Parser::step = Step::drop_table;
    } else {
        Parser::step = Step::error;
        Parser::error_message = "Invalid Query type";
        return;
    }
}