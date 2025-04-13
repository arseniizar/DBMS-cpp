#include <string>
#include <gtest/gtest.h>

#include "../engine/parser/Parser.hpp"

TEST(ParserTest, ParseDropTableValid) {
    Parser parser;
    const std::string input = "DROP TABLE my_table;";
    parser.input(input);

    auto [query, error] = parser.parser_switch();

    EXPECT_TRUE(error.message.empty()) << "Expected no error, but got: " << error.message;

    EXPECT_EQ(query.get_command(), "DROP TABLE my_table;");
}
