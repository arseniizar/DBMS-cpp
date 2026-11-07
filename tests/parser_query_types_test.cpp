//
// Created by Arsenii Zarudniuk on 01/12/2025.
//
#include <gtest/gtest.h>
#include "engine/parser/Parser.hpp"
#include "engine/query/Query.hpp"

class ParserTest : public ::testing::Test {
protected:
    Parser parser;
};


TEST_F(ParserTest, HandlesBasicCreateTable) {
    std::string sql = "CREATE TABLE users (id INTEGER, name NVARCHAR2)";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Create);
    EXPECT_EQ(q.get_table_name(), "users");
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesCreateTableWithPrimaryKey) {
    std::string sql = "CREATE TABLE products (product_id INTEGER PRIMARY KEY, price INTEGER)";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Create);
    EXPECT_EQ(q.get_table_name(), "products");

    auto pk = q.get_primary_key();
    EXPECT_EQ(pk.value, "product_id");
    EXPECT_EQ(pk.k_a.k_t, Key_type::PK);

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesBasicDropTable) {
    std::string sql = "DROP TABLE customers";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Drop);
    EXPECT_EQ(q.get_table_name(), "customers");
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesBasicInsertInto) {
    std::string sql = "INSERT INTO users (id, name) VALUES ('1', 'Arsenii')";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Insert);
    EXPECT_EQ(q.get_table_name(), "users");

    auto fields = q.get_insert_fields();
    ASSERT_EQ(fields.size(), 2);
    EXPECT_EQ(fields[0].value, "id");
    EXPECT_EQ(fields[1].value, "name");

    auto inserts = q.get_inserts();
    ASSERT_EQ(inserts.size(), 1);
    ASSERT_EQ(inserts[0].size(), 2);
    EXPECT_EQ(inserts[0][0].value, "1");
    EXPECT_EQ(inserts[0][1].value, "Arsenii");

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesMultiValueInsert) {
    std::string sql = "INSERT INTO users (id) VALUES ('1'), ('2'), ('3')";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Insert);
    EXPECT_EQ(q.get_table_name(), "users");

    auto inserts = q.get_inserts();
    ASSERT_EQ(inserts.size(), 3);
    EXPECT_EQ(inserts[0][0].value, "1");
    EXPECT_EQ(inserts[1][0].value, "2");
    EXPECT_EQ(inserts[2][0].value, "3");

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectAll) {
    std::string sql = "SELECT * FROM users";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    EXPECT_EQ(q.get_table_name(), "users");

    auto fields = q.get_fields();
    ASSERT_EQ(fields.size(), 1);
    EXPECT_EQ(fields[0].value, "*");

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectSpecificColumns) {
    std::string sql = "SELECT id, name FROM users";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    EXPECT_EQ(q.get_table_name(), "users");

    auto fields = q.get_fields();
    ASSERT_EQ(fields.size(), 2);
    EXPECT_EQ(fields[0].value, "id");
    EXPECT_EQ(fields[1].value, "name");

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesBasicDeleteFrom) {
    std::string sql = "DELETE FROM orders";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Delete);
    EXPECT_EQ(q.get_table_name(), "orders");
    EXPECT_TRUE(q.get_conditions().empty());

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnInvalidQueryType) {
    std::string sql = "MODIFY TABLE users";
    parser.input(sql);
    Query q = parser.parse();

    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnIncompleteCreateTable) {
    std::string sql = "CREATE TABLE users (";
    parser.input(sql);
    Query q = parser.parse();

    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesQueryWithMixedCase) {
    std::string sql = "cReAtE tAbLe MiXeDcAsE (id INTEGER)";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Create);
    EXPECT_EQ(q.get_table_name(), "MiXeDcAsE"); // Ваш парсер, схоже, не змінює регістр, що є нормальним
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesQueryWithExtraWhitespace) {
    std::string sql = "   INSERT   INTO    users    (  id   , name  )   VALUES   ( '1' ,  'Extra Space'  )   ";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Insert);
    EXPECT_EQ(q.get_table_name(), "users");

    auto fields = q.get_insert_fields();
    ASSERT_EQ(fields.size(), 2);
    EXPECT_EQ(fields[0].value, "id");
    EXPECT_EQ(fields[1].value, "name");

    auto inserts = q.get_inserts();
    ASSERT_EQ(inserts.size(), 1);
    ASSERT_EQ(inserts[0].size(), 2);
    EXPECT_EQ(inserts[0][0].value, "1");
    EXPECT_EQ(inserts[0][1].value, "Extra Space");

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesInsertIntoWithoutSpecifyingColumns) {
    std::string sql = "INSERT INTO products VALUES ('101', 'Book', '250')";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Insert);
    EXPECT_EQ(q.get_table_name(), "products");

    EXPECT_TRUE(q.get_insert_fields().empty());

    auto inserts = q.get_inserts();
    ASSERT_EQ(inserts.size(), 1);
    ASSERT_EQ(inserts[0].size(), 3);
    EXPECT_EQ(inserts[0][0].value, "101");
    EXPECT_EQ(inserts[0][1].value, "Book");
    EXPECT_EQ(inserts[0][2].value, "250");

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesCreateTableWithForeignKey) {
    std::string sql = "CREATE TABLE orders (order_id INTEGER, user_id INTEGER FOREIGN KEY REFERENCES users(id))";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Create);
    EXPECT_EQ(q.get_table_name(), "orders");

    auto fks = q.get_foreign_keys();
    ASSERT_EQ(fks.size(), 1);
    EXPECT_EQ(fks[0].value, "user_id");
    EXPECT_EQ(fks[0].k_a.k_t, Key_type::FK);
    EXPECT_EQ(q.get_referenced_table_name(), "users");

    auto ref_fields = q.get_referenced_fields_names();
    ASSERT_EQ(ref_fields.size(), 1);
    EXPECT_EQ(ref_fields[0], "id");

    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnDropTableWithExtraWords) {
    std::string sql = "DROP TABLE users now";
    parser.input(sql);
    Query q = parser.parse();

    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnCreateTableWithTwoPrimaryKeys) {
    std::string sql = "CREATE TABLE users (id INTEGER PRIMARY KEY, email NVARCHAR2 PRIMARY KEY)";
    parser.input(sql);
    parser.parse();
    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, SucceedsOnForeignKeySyntax) {
    std::string sql = "CREATE TABLE orders (id INTEGER, user_id INTEGER FOREIGN KEY REFERENCES users(user_id))";
    parser.input(sql);
    parser.parse();
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, SucceedsOnMismatchedDataTypeSyntax) {
    std::string sql = "INSERT INTO users (id) VALUES ('this-is-not-an-integer')";
    parser.input(sql);
    parser.parse();
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnInsertWithEmptyValues) {
    std::string sql = "INSERT INTO users (id) VALUES ()";
    parser.input(sql);
    parser.parse();
    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesDeleteWithWhereClause) {
    std::string sql = "DELETE FROM users WHERE id = '1'";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Delete);
    EXPECT_EQ(parser.get_error().message, "");

    auto conditions = q.get_conditions();
    ASSERT_EQ(conditions.size(), 1);
    EXPECT_EQ(conditions[0].get_operand1(), "id");
    EXPECT_EQ(conditions[0].get_operator(), Query_operator::eq);
    EXPECT_EQ(conditions[0].get_operand2(), "1");
}

TEST_F(ParserTest, HandlesBasicUpdate) {
    std::string sql = "UPDATE users SET name = 'Arseniy' WHERE id = '1'";
    parser.input(sql);
    Query q = parser.parse();

    ASSERT_EQ(q.get_query_type(), Query_type::Update);
    EXPECT_EQ(parser.get_error().message, "");
    EXPECT_EQ(q.get_table_name(), "users");

    auto updates = q.get_updates();
    ASSERT_EQ(updates.size(), 1);
    EXPECT_EQ(updates[0].field_to_update.value, "name");
    EXPECT_EQ(updates[0].new_value, "Arseniy");

    auto conditions = q.get_conditions();
    ASSERT_EQ(conditions.size(), 1);
    EXPECT_EQ(conditions[0].get_operand1(), "id");
    EXPECT_EQ(conditions[0].get_operand2(), "1");
}

TEST_F(ParserTest, HandlesSelectWithInnerJoin) {
    parser.input("SELECT name, product FROM users JOIN orders ON user_id = customer_id");
    Query q = parser.parse();
    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    EXPECT_EQ(q.get_table_name(), "users");
    EXPECT_EQ(q.get_joined_table_name(), "orders");
    EXPECT_EQ(q.get_join_type(), join_type::JOIN);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithLeftJoin) {
    parser.input("SELECT name, product FROM users LEFT JOIN orders ON user_id = customer_id");
    Query q = parser.parse();
    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    EXPECT_EQ(q.get_joined_table_name(), "orders");
    EXPECT_EQ(q.get_join_type(), join_type::LEFT_JOIN);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithRightJoin) {
    parser.input("SELECT name FROM users RIGHT JOIN orders ON user_id = customer_id");
    Query q = parser.parse();
    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    EXPECT_EQ(q.get_join_type(), join_type::RIGHT_JOIN);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithFullJoin) {
    parser.input("SELECT name FROM users FULL JOIN orders ON user_id = customer_id");
    Query q = parser.parse();
    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    EXPECT_EQ(q.get_join_type(), join_type::FULL_JOIN);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithGroupBy) {
    parser.input("SELECT city FROM sales GROUP BY city");
    Query q = parser.parse();
    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    ASSERT_EQ(q.get_group_by_columns().size(), 1);
    EXPECT_EQ(q.get_group_by_columns()[0], "city");
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithGroupByAndHaving) {
    parser.input("SELECT city, COUNT(*) FROM sales GROUP BY city HAVING COUNT(*) > 2");
    Query q = parser.parse();
    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    ASSERT_EQ(q.get_group_by_columns().size(), 1);
    ASSERT_EQ(q.get_conditions().size(), 1);
    EXPECT_EQ(q.get_conditions()[0].c_type, Condition_type::HAVING);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithCountFunction) {
    parser.input("SELECT COUNT(*) FROM users");
    Query q = parser.parse();
    ASSERT_EQ(q.get_fields().size(), 1);
    EXPECT_EQ(q.get_fields()[0].agg_t, Aggregation_type::COUNT);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithSumFunction) {
    parser.input("SELECT SUM(amount) FROM sales GROUP BY city");
    Query q = parser.parse();
    ASSERT_EQ(q.get_fields().size(), 1);
    EXPECT_EQ(q.get_fields()[0].agg_t, Aggregation_type::SUM);
    EXPECT_EQ(q.get_fields()[0].value, "amount");
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithMinAndMaxFunctions) {
    parser.input("SELECT MIN(price), MAX(price) FROM products GROUP BY category");
    Query q = parser.parse();
    ASSERT_EQ(q.get_fields().size(), 2);
    EXPECT_EQ(q.get_fields()[0].agg_t, Aggregation_type::MIN);
    EXPECT_EQ(q.get_fields()[1].agg_t, Aggregation_type::MAX);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesWhereWithNotEqualOperator) {
    parser.input("SELECT * FROM users WHERE city != 'Kyiv'");
    Query q = parser.parse();
    ASSERT_EQ(q.get_conditions().size(), 1);
    EXPECT_EQ(q.get_conditions()[0].get_operator(), Query_operator::ne);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesWhereWithGreaterAndLessOperators) {
    parser.input("SELECT * FROM products WHERE price > '10' AND price < '100'");
    Query q = parser.parse();
    ASSERT_EQ(q.get_conditions().size(), 2);
    EXPECT_EQ(q.get_conditions()[0].get_operator(), Query_operator::gt);
    EXPECT_EQ(q.get_conditions()[1].get_operator(), Query_operator::lt);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesWhereWithGreaterOrEqualAndLessOrEqual) {
    parser.input("SELECT * FROM products WHERE price >= '10' AND price <= '100'");
    Query q = parser.parse();
    ASSERT_EQ(q.get_conditions().size(), 2);
    EXPECT_EQ(q.get_conditions()[0].get_operator(), Query_operator::gte);
    EXPECT_EQ(q.get_conditions()[1].get_operator(), Query_operator::lte);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesWhereWithMultipleAndConditions) {
    parser.input("SELECT id FROM users WHERE city = 'Warsaw' AND role = 'Engineer' AND active = '1'");
    Query q = parser.parse();
    ASSERT_EQ(q.get_conditions().size(), 3);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesTrailingSemicolonInSelect) {
    parser.input("SELECT * FROM users;");
    Query q = parser.parse();
    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    EXPECT_EQ(q.get_table_name(), "users");
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, HandlesSelectWithThreeColumns) {
    parser.input("SELECT id, name, city FROM users");
    Query q = parser.parse();
    ASSERT_EQ(q.get_query_type(), Query_type::Select);
    EXPECT_EQ(q.get_fields().size(), 3);
    EXPECT_EQ(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnDuplicateColumnNameInCreateTable) {
    parser.input("CREATE TABLE users (id INTEGER, id NVARCHAR2)");
    parser.parse();
    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnInvalidDataTypeInCreateTable) {
    parser.input("CREATE TABLE users (id UNKNOWN_TYPE)");
    parser.parse();
    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnSelectWithoutFromKeyword) {
    parser.input("SELECT id users");
    parser.parse();
    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnSelectWithoutTableName) {
    parser.input("SELECT id FROM");
    parser.parse();
    EXPECT_NE(parser.get_error().message, "");
}

TEST_F(ParserTest, FailsOnUpdateWithoutSetKeyword) {
    parser.input("UPDATE users name = 'Arsenii'");
    parser.parse();
    EXPECT_NE(parser.get_error().message, "");
}