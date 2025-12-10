//
// Created by Arsenii Zarudniuk on 08/12/2025.
//

#include "gtest/gtest.h"
#include "engine/Dbms/Dbms.hpp"

class DbmsUpdateTest : public ::testing::Test {
protected:
    Dbms dbms;

    void SetUp() override {
        dbms.process_query_to_string("CREATE TABLE users (id INTEGER, name NVARCHAR2, city NVARCHAR2)");
        dbms.process_query_to_string("INSERT INTO users (id, name, city) VALUES ('1', 'Arsenii', 'Kyiv')");
        dbms.process_query_to_string("INSERT INTO users (id, name, city) VALUES ('2', 'Olena', 'Lviv')");
        dbms.process_query_to_string("INSERT INTO users (id, name, city) VALUES ('3', 'Max', 'Kyiv')");
    }
};

TEST_F(DbmsUpdateTest, HandlesBasicUpdateWithWhere) {
    std::string update_result = dbms.process_query_to_string("UPDATE users SET name = 'Yana' WHERE id = '2'");
    EXPECT_EQ(update_result, "Query executed successfully");

    std::string select_result = dbms.process_query_to_string("SELECT name FROM users WHERE id = '2'");
    EXPECT_NE(select_result.find("Yana"), std::string::npos);
    EXPECT_EQ(select_result.find("Olena"), std::string::npos);
}

TEST_F(DbmsUpdateTest, HandlesMultipleFieldsUpdate) {
    dbms.process_query_to_string("UPDATE users SET name = 'Petro', city = 'Odesa' WHERE id = '1'");

    std::string select_result = dbms.process_query_to_string("SELECT name, city FROM users WHERE id = '1'");
    EXPECT_NE(select_result.find("Petro"), std::string::npos);
    EXPECT_NE(select_result.find("Odesa"), std::string::npos);
    EXPECT_EQ(select_result.find("Arsenii"), std::string::npos);
    EXPECT_EQ(select_result.find("Kyiv"), std::string::npos);
}

TEST_F(DbmsUpdateTest, HandlesUpdateOfMultipleRows) {
    dbms.process_query_to_string("UPDATE users SET city = 'Capital' WHERE city = 'Kyiv'");

    std::string select_result = dbms.process_query_to_string("SELECT id FROM users WHERE city = 'Capital'");
    EXPECT_NE(select_result.find("1"), std::string::npos);
    EXPECT_NE(select_result.find("3"), std::string::npos);

    std::string lviv_result = dbms.process_query_to_string("SELECT city FROM users WHERE id = '2'");
    EXPECT_NE(lviv_result.find("Lviv"), std::string::npos);
}

TEST_F(DbmsUpdateTest, HandlesUpdateWithoutWhere) {
    dbms.process_query_to_string("UPDATE users SET city = 'Ukraine'");

    std::string select_result = dbms.process_query_to_string("SELECT id FROM users WHERE city = 'Ukraine'");
    EXPECT_NE(select_result.find("1"), std::string::npos);
    EXPECT_NE(select_result.find("2"), std::string::npos);
    EXPECT_NE(select_result.find("3"), std::string::npos);
}

TEST_F(DbmsUpdateTest, FailsOnUpdateNonExistentTable) {
    std::string result = dbms.process_query_to_string("UPDATE non_existent_table SET name = 'Error'");
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    EXPECT_NE(result.find("error"), std::string::npos);
    EXPECT_NE(result.find("does not exist"), std::string::npos);
}

TEST_F(DbmsUpdateTest, FailsOnUpdateNonExistentColumn) {
    std::string result = dbms.process_query_to_string("UPDATE users SET non_existent_column = 'Error' WHERE id = '1'");
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    EXPECT_NE(result.find("error"), std::string::npos);
    EXPECT_NE(result.find("column"), std::string::npos);
    EXPECT_NE(result.find("does not exist"), std::string::npos);
}

TEST_F(DbmsUpdateTest, FailsOnUpdateWithMismatchedDataType) {
    std::string result = dbms.process_query_to_string("UPDATE users SET id = 'not-a-number' WHERE id = '1'");
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    EXPECT_NE(result.find("error"), std::string::npos);
    EXPECT_NE(result.find("data type mismatch"), std::string::npos);
}
