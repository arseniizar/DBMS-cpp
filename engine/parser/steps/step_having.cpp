#include "../Parser.hpp"
#include "../../utils/ut_is_identifier/ut_is_identifier.hpp"
#include "../../utils/ut_str_toupper/ut_str_toupper.hpp"

void Parser::step_having() {
    auto having_token = pop();
    str_toupper(having_token);
    if (having_token != "HAVING") {
        step = Step::error;
        error_message = "Internal parser error: expected 'HAVING'";
        return;
    }

    step = Step::having_field;
}

void Parser::step_having_field() {
    fmt::println("  [Executing step_having_field]"); // Лог 1: Початок функції

    std::string next_token = peek();
    std::string upper_token = next_token;
    str_toupper(upper_token);
    fmt::println("  -> Peeked token in step_having_field: '{}'", next_token); // Лог 2: Що ми побачили

    std::string condition_operand1;

    if (upper_token == "COUNT") {
        fmt::println("  -> Token is 'COUNT'. Entering COUNT block."); // Лог 3: Заходимо в блок COUNT
        pop(); // Споживаємо "COUNT"

        if (peek() != "(") {
            fmt::println("  -> ERROR: Expected '(', but got '{}'", peek()); // Лог помилки
            step = Step::error;
            error_message = "at HAVING: expected '(' after COUNT";
            return;
        }
        pop(); // Споживаємо '('

        std::string content = pop();
        if (!is_identifier_or_asterisk(content)) {
            fmt::println("  -> ERROR: Expected identifier or '*', but got '{}'", content); // Лог помилки
            step = Step::error;
            error_message = "at HAVING: expected column name or '*' inside COUNT()";
            return;
        }

        if (peek() != ")") {
            fmt::println("  -> ERROR: Expected ')', but got '{}'", peek()); // Лог помилки
            step = Step::error;
            error_message = "at HAVING: expected ')' after content in COUNT()";
            return;
        }
        pop(); // Споживаємо ')'

        condition_operand1 = "COUNT(" + content + ")";

    } else if (is_identifier(next_token)) {
        fmt::println("  -> Token is an identifier. Popping '{}'.", next_token); // Лог 4: Це ідентифікатор
        condition_operand1 = pop();
    } else {
        fmt::println("  -> Token is NEITHER 'COUNT' NOR an identifier. Setting to ERROR."); // Лог 5: Це не те, що нам потрібно
        step = Step::error;
        error_message = "at HAVING: expected column or aggregate function";
        return;
    }

    fmt::println("  -> Successfully parsed having field. Setting condition and moving to 'having_operator'."); // Лог 6: Успіх
    q.append_condition(Condition(condition_operand1, true, Condition_type::HAVING));
    step = Step::having_operator;
}

void Parser::step_having_operator() {
    if (!peek_is_operator("at HAVING: expected an operator")) {
        return;
    }
    step = Step::having_value;
}

void Parser::step_having_value() {
    auto value = peek();
    if (value.empty()) {
        step = Step::error;
        error_message = "at HAVING: expected a value after operator";
        return;
    }
    pop();

    auto curr_cond = q.get_current_condition();
    curr_cond.operand2 = value;
    curr_cond.operand2_is_field = false;
    q.set_current_condition(curr_cond);

    if (peek().empty()) {
        pop_flag = true;
    } else {
        step = Step::error;
        error_message = "Unexpected token after HAVING clause";
    }
}
