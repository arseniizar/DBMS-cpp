#include "../Parser.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void Parser::step_type() {
    auto peeked = Parser::peek();
    str_toupper(peeked);

    if (peeked == "SELECT") {
        Parser::q.set_query_type(Query_type::Select);
        Parser::pop();
        Parser::step = Step::select_field;
    }
    else if (peeked == "INSERT") {
        Parser::pop();
        auto next_token = Parser::peek();
        str_toupper(next_token);
        if (next_token != "INTO") {
            Parser::step = Step::error;
            Parser::error_message = "at INSERT: expected 'INTO'";
            return;
        }
        Parser::pop();
        Parser::q.set_query_type(Query_type::Insert);
        Parser::step = Step::insert_table;
    }
    else if (peeked == "DELETE") {
        Parser::pop();
        auto next_token = Parser::peek();
        str_toupper(next_token);
        if (next_token != "FROM") {
            Parser::step = Step::error;
            Parser::error_message = "at DELETE: expected 'FROM'";
            return;
        }
        Parser::pop();
        Parser::q.set_query_type(Query_type::Delete);
        Parser::step = Step::delete_from_table;
    }
    else if (peeked == "CREATE") {
        Parser::pop();
        auto next_token = Parser::peek();
        str_toupper(next_token);
        if (next_token != "TABLE") {
            Parser::step = Step::error;
            Parser::error_message = "at CREATE: expected 'TABLE'";
            return;
        }
        Parser::pop();
        Parser::q.set_query_type(Query_type::Create);
        Parser::step = Step::create_table;
    }
    else if (peeked == "DROP") {
        Parser::pop();
        auto next_token = Parser::peek();
        str_toupper(next_token);
        if (next_token != "TABLE") {
            Parser::step = Step::error;
            Parser::error_message = "at DROP: expected 'TABLE'";
            return;
        }
        Parser::pop();
        Parser::q.set_query_type(Query_type::Drop);
        Parser::step = Step::drop_table;
    }
    else if (peeked == "UPDATE") {
        Parser::pop();
        Parser::q.set_query_type(Query_type::Update);
        Parser::step = Step::update_table;
    }
    else {
        Parser::step = Step::error;
        Parser::error_message = "Invalid Query type";
        return;
    }
}
