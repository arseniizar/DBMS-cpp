//
// Created by Arsenii Zarudniuk on 07/12/2025.
//

#include <gtest/gtest.h>
#include "Dbms/Dbms.hpp"

class DbmsIntegrationTest : public ::testing::Test {
protected:
    Dbms dbms;
};

TEST_F(DbmsIntegrationTest, CreateTableSuccessfully) {
    std::string result = dbms.process_query("CREATE TABLE users (id INTEGER)");
    EXPECT_EQ(dbms.process_query("-tables"), "users\n");
}

TEST_F(DbmsIntegrationTest, FailsOnCreatingExistingTable) {
    dbms.process_query("CREATE TABLE users (id INTEGER)");
    std::string result = dbms.process_query("CREATE TABLE users (id INTEGER)");
    EXPECT_NE(result.find("already exists"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, DropTableSuccessfully) {
    dbms.process_query("CREATE TABLE users (id INTEGER)");
    dbms.process_query("DROP TABLE users");
    EXPECT_EQ(dbms.process_query("-tables"), "[ *empty* ]");
}

TEST_F(DbmsIntegrationTest, FailsOnDroppingNonExistentTable) {
    std::string result = dbms.process_query("DROP TABLE non_existent");
    EXPECT_NE(result.find("nonexistent table"), std::string::npos);
}


TEST_F(DbmsIntegrationTest, InsertSuccessfullyAddsData) {
    dbms.process_query("CREATE TABLE users (id INTEGER, name NVARCHAR2)");
    dbms.process_query("INSERT INTO users (id, name) VALUES ('123', 'Arsenii')");

    std::string result = dbms.process_query("SELECT * FROM users");
    EXPECT_NE(result.find("123"), std::string::npos);
    EXPECT_NE(result.find("Arsenii"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FullCycleCreateInsertSelect) {
    std::cout << "TEST" << std::endl;
    dbms.process_query("CREATE TABLE data (key NVARCHAR2, value INTEGER)");
    dbms.process_query("INSERT INTO data (key, value) VALUES ('test', '100')");

    std::string result = dbms.process_query("SELECT value FROM data WHERE key = 'test'");
    EXPECT_NE(result.find("100"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnInsertWithMismatchedColumnCount) {
    dbms.process_query("CREATE TABLE users (id INTEGER, name NVARCHAR2)");
    std::string result = dbms.process_query("INSERT INTO users (id) VALUES ('1', 'superfluous name')");
    EXPECT_NE(result.find("at INSERT: size of inserts doesn't correspond to a number of fields in the table"),
              std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnInsertWithMismatchedDataType) {
    dbms.process_query("CREATE TABLE products (price INTEGER)");
    std::string result = dbms.process_query("INSERT INTO products (price) VALUES ('cheap')");
    EXPECT_NE(result.find("improper data type"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, DeleteWithoutWhereClearsTable) {
    dbms.process_query("CREATE TABLE items (id INTEGER)");
    dbms.process_query("INSERT INTO items (id) VALUES ('1'), ('2')");
    dbms.process_query("DELETE FROM items");

    std::string result = dbms.process_query("SELECT * FROM items");
    EXPECT_EQ(result.find("1"), std::string::npos);
    EXPECT_EQ(result.find("2"), std::string::npos);
}


TEST_F(DbmsIntegrationTest, UiCommandTablesOnEmptyDb) {
    std::string result = dbms.process_query("-tables");
    EXPECT_EQ(result, "[ *empty* ]");
}

TEST_F(DbmsIntegrationTest, UiCommandTablesWithOneTable) {
    dbms.process_query("CREATE TABLE my_table (id INTEGER)");
    std::string result = dbms.process_query("-tables");
    EXPECT_EQ(result, "my_table\n");
}

TEST_F(DbmsIntegrationTest, UiCommandHelp) {
    std::string result = dbms.process_query("-help");
    EXPECT_NE(result.find("Available commands"), std::string::npos);
}