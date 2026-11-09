//
// Created by Arsenii Zarudniuk on 07/11/2026.
//

#include "gtest/gtest.h"
#include "engine/Dbms/Dbms.hpp"

class DbmsJoinTest : public ::testing::Test {
protected:
    Dbms dbms;

    void SetUp() override {
        dbms.process_query_to_string("CREATE TABLE users (user_id INTEGER, name NVARCHAR2, city NVARCHAR2)");
        dbms.process_query_to_string("CREATE TABLE orders (order_id INTEGER, customer_id INTEGER, product NVARCHAR2)");

        dbms.process_query_to_string("INSERT INTO users (user_id, name, city) VALUES ('1', 'Arsenii', 'Warsaw')");
        dbms.process_query_to_string("INSERT INTO users (user_id, name, city) VALUES ('2', 'Olena', 'Kyiv')");
        dbms.process_query_to_string("INSERT INTO users (user_id, name, city) VALUES ('3', 'Max', 'Lviv')");

        dbms.process_query_to_string("INSERT INTO orders (order_id, customer_id, product) VALUES ('101', '1', 'Laptop')");
        dbms.process_query_to_string("INSERT INTO orders (order_id, customer_id, product) VALUES ('102', '1', 'Keyboard')");
        dbms.process_query_to_string("INSERT INTO orders (order_id, customer_id, product) VALUES ('103', '2', 'Monitor')");
    }
};

TEST_F(DbmsJoinTest, HandlesInnerJoinSelectColumns) {
    std::string result = dbms.process_query_to_string(
        "SELECT name, product FROM users JOIN orders ON user_id = customer_id"
    );

    EXPECT_NE(result.find("Arsenii"), std::string::npos);
    EXPECT_NE(result.find("Laptop"), std::string::npos);
    EXPECT_NE(result.find("Keyboard"), std::string::npos);
    EXPECT_NE(result.find("Olena"), std::string::npos);
    EXPECT_NE(result.find("Monitor"), std::string::npos);
    EXPECT_EQ(result.find("Max"), std::string::npos);
}

TEST_F(DbmsJoinTest, HandlesLeftJoinPreservingUnmatchedRows) {
    std::string result = dbms.process_query_to_string(
        "SELECT name, product FROM users LEFT JOIN orders ON user_id = customer_id"
    );

    EXPECT_NE(result.find("Arsenii"), std::string::npos);
    EXPECT_NE(result.find("Olena"), std::string::npos);
    EXPECT_NE(result.find("Max"), std::string::npos);
}

TEST_F(DbmsJoinTest, HandlesJoinWithWhereAndGroupBy) {
    std::string result = dbms.process_query_to_string(
        "SELECT name, COUNT(*) FROM users JOIN orders ON user_id = customer_id WHERE city = 'Warsaw' GROUP BY name"
    );

    EXPECT_NE(result.find("Arsenii"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_EQ(result.find("Olena"), std::string::npos);
}

TEST_F(DbmsJoinTest, HandlesMultiConditionWhereClause) {
    std::string result = dbms.process_query_to_string(
        "SELECT name FROM users WHERE city = 'Warsaw' AND user_id = '1'"
    );

    EXPECT_NE(result.find("Arsenii"), std::string::npos);
    EXPECT_EQ(result.find("Olena"), std::string::npos);
}

TEST_F(DbmsJoinTest, HandlesInnerJoinSelectAllColumns) {
    std::string result = dbms.process_query_to_string(
        "SELECT * FROM users JOIN orders ON user_id = customer_id"
    );

    EXPECT_NE(result.find("Arsenii"), std::string::npos);
    EXPECT_NE(result.find("Laptop"), std::string::npos);
    EXPECT_NE(result.find("Olena"), std::string::npos);
    EXPECT_EQ(result.find("Max"), std::string::npos);
}

TEST_F(DbmsJoinTest, HandlesInnerJoinReversedOnConditionOrder) {
    std::string result = dbms.process_query_to_string(
        "SELECT name, product FROM users JOIN orders ON customer_id = user_id"
    );

    EXPECT_NE(result.find("Arsenii"), std::string::npos);
    EXPECT_NE(result.find("Laptop"), std::string::npos);
    EXPECT_NE(result.find("Monitor"), std::string::npos);
}

TEST_F(DbmsJoinTest, HandlesLeftJoinWhenNoRowsMatch) {
    dbms.process_query_to_string("CREATE TABLE empty_orders (order_id INTEGER, customer_id INTEGER)");
    std::string result = dbms.process_query_to_string(
        "SELECT name FROM users LEFT JOIN empty_orders ON user_id = customer_id"
    );

    EXPECT_NE(result.find("Arsenii"), std::string::npos);
    EXPECT_NE(result.find("Olena"), std::string::npos);
    EXPECT_NE(result.find("Max"), std::string::npos);
}

TEST_F(DbmsJoinTest, FailsOnJoiningNonExistentTable) {
    std::string result = dbms.process_query_to_string(
        "SELECT name FROM users JOIN missing_table ON user_id = customer_id"
    );

    EXPECT_NE(result.find("nonexistent table"), std::string::npos);
}

TEST_F(DbmsJoinTest, FailsOnJoinWithNonExistentColumn) {
    std::string result = dbms.process_query_to_string(
        "SELECT name FROM users JOIN orders ON user_id = wrong_col"
    );

    EXPECT_NE(result.find("nonexistent column"), std::string::npos);
}

TEST_F(DbmsJoinTest, HandlesJoinWithSumAggregation) {
    dbms.process_query_to_string("CREATE TABLE invoices (inv_id INTEGER, cust_id INTEGER, total INTEGER)");
    dbms.process_query_to_string("INSERT INTO invoices (inv_id, cust_id, total) VALUES ('1', '1', '300')");
    dbms.process_query_to_string("INSERT INTO invoices (inv_id, cust_id, total) VALUES ('2', '1', '450')");
    dbms.process_query_to_string("INSERT INTO invoices (inv_id, cust_id, total) VALUES ('3', '2', '200')");

    std::string result = dbms.process_query_to_string(
        "SELECT name, SUM(total) FROM users JOIN invoices ON user_id = cust_id GROUP BY name"
    );

    EXPECT_NE(result.find("Arsenii"), std::string::npos);
    EXPECT_NE(result.find("750"), std::string::npos);
    EXPECT_NE(result.find("Olena"), std::string::npos);
    EXPECT_NE(result.find("200"), std::string::npos);
}
