//
// Created by Arsenii Zarudniuk on 01/12/2025.
//
#include <gtest/gtest.h>
#include "parser/Parser.hpp"
#include "query/Query.hpp"

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