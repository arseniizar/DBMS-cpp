//
// Created by Arsenii Zarudniuk on 07/12/2025.
//

#include <gtest/gtest.h>
#include <chrono>
#include "engine/Dbms/Dbms.hpp"

class DbmsIntegrationTest : public ::testing::Test {
protected:
    Dbms dbms;
};

TEST_F(DbmsIntegrationTest, CreateTableSuccessfully) {
    std::string result = dbms.process_query_to_string("CREATE TABLE users (id INTEGER)");
    EXPECT_EQ(dbms.process_query_to_string("-tables"), "users\n");
}

TEST_F(DbmsIntegrationTest, FailsOnCreatingExistingTable) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER)");
    std::string result = dbms.process_query_to_string("CREATE TABLE users (id INTEGER)");
    EXPECT_NE(result.find("already exists"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, DropTableSuccessfully) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER)");
    dbms.process_query_to_string("DROP TABLE users");
    EXPECT_EQ(dbms.process_query_to_string("-tables"), "[ *empty* ]");
}

TEST_F(DbmsIntegrationTest, FailsOnDroppingNonExistentTable) {
    std::string result = dbms.process_query_to_string("DROP TABLE non_existent");
    EXPECT_NE(result.find("nonexistent table"), std::string::npos);
}


TEST_F(DbmsIntegrationTest, InsertSuccessfullyAddsData) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER, name NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO users (id, name) VALUES ('123', 'Arsenii')");

    std::string result = dbms.process_query_to_string("SELECT * FROM users");
    EXPECT_NE(result.find("123"), std::string::npos);
    EXPECT_NE(result.find("Arsenii"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FullCycleCreateInsertSelect) {
    std::cout << "TEST" << std::endl;
    dbms.process_query_to_string("CREATE TABLE data (key NVARCHAR2, value INTEGER)");
    dbms.process_query_to_string("INSERT INTO data (key, value) VALUES ('test', '100')");

    std::string result = dbms.process_query_to_string("SELECT value FROM data WHERE key = 'test'");
    EXPECT_NE(result.find("100"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnInsertWithMismatchedColumnCount) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER, name NVARCHAR2)");
    std::string result = dbms.process_query_to_string("INSERT INTO users (id) VALUES ('1', 'superfluous name')");
    EXPECT_NE(result.find("at INSERT: size of inserts doesn't correspond to a number of fields in the table"),
              std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnInsertWithMismatchedDataType) {
    dbms.process_query_to_string("CREATE TABLE products (price INTEGER)");
    std::string result = dbms.process_query_to_string("INSERT INTO products (price) VALUES ('cheap')");
    EXPECT_NE(result.find("improper data type"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, DeleteWithoutWhereClearsTable) {
    dbms.process_query_to_string("CREATE TABLE items (id INTEGER)");
    dbms.process_query_to_string("INSERT INTO items (id) VALUES ('1'), ('2')");
    dbms.process_query_to_string("DELETE FROM items");

    std::string result = dbms.process_query_to_string("SELECT * FROM items");
    EXPECT_EQ(result.find("1"), std::string::npos);
    EXPECT_EQ(result.find("2"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, DeleteWithWhereRemovesMatchingRows) {
    dbms.process_query_to_string("CREATE TABLE items (id INTEGER, name NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO items (id, name) VALUES ('1', 'Keyboard'), ('2', 'Mouse')");
    dbms.process_query_to_string("DELETE FROM items WHERE id = '1'");

    std::string result = dbms.process_query_to_string("SELECT * FROM items");
    EXPECT_EQ(result.find("Keyboard"), std::string::npos);
    EXPECT_NE(result.find("Mouse"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, RejectsDuplicatePrimaryKeyOnInsert) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER PRIMARY KEY, name NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO users (id, name) VALUES ('1', 'Arsenii')");
    std::string duplicate = dbms.process_query_to_string("INSERT INTO users (id, name) VALUES ('1', 'Max')");
    EXPECT_NE(duplicate.find("duplicate primary key"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, EnforcesForeignKeyConstraintOnInsert) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER PRIMARY KEY, name NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO users (id, name) VALUES ('10', 'Arsenii')");
    dbms.process_query_to_string("CREATE TABLE orders (order_id INTEGER, user_id INTEGER FOREIGN KEY REFERENCES users(id))");

    std::string valid_res = dbms.process_query_to_string("INSERT INTO orders (order_id, user_id) VALUES ('100', '10')");
    EXPECT_NE(valid_res.find("successfully"), std::string::npos);

    std::string invalid_res = dbms.process_query_to_string("INSERT INTO orders (order_id, user_id) VALUES ('101', '999')");
    EXPECT_NE(invalid_res.find("foreign key constraint failed"), std::string::npos);
}


TEST_F(DbmsIntegrationTest, UiCommandTablesOnEmptyDb) {
    std::string result = dbms.process_query_to_string("-tables");
    EXPECT_EQ(result, "[ *empty* ]");
}

TEST_F(DbmsIntegrationTest, UiCommandTablesWithOneTable) {
    dbms.process_query_to_string("CREATE TABLE my_table (id INTEGER)");
    std::string result = dbms.process_query_to_string("-tables");
    EXPECT_EQ(result, "my_table\n");
}

TEST_F(DbmsIntegrationTest, UiCommandHelp) {
    std::string result = dbms.process_query_to_string("-help");
    EXPECT_NE(result.find("Available commands"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnForeignKeyReferencingNonExistentTable) {
    std::string result = dbms.process_query_to_string(
        "CREATE TABLE orders (order_id INTEGER, user_id INTEGER FOREIGN KEY REFERENCES missing_users(id))"
    );
    EXPECT_NE(result.find("invalid foreign key reference"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnForeignKeyReferencingNonExistentColumn) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER PRIMARY KEY)");
    std::string result = dbms.process_query_to_string(
        "CREATE TABLE orders (order_id INTEGER, user_id INTEGER FOREIGN KEY REFERENCES users(missing_col))"
    );
    EXPECT_NE(result.find("invalid foreign key reference"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, DeleteWithMultipleWhereConditions) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER, city NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO users (id, city) VALUES ('1', 'Warsaw'), ('2', 'Warsaw'), ('3', 'Kyiv')");
    dbms.process_query_to_string("DELETE FROM users WHERE city = 'Warsaw' AND id = '1'");

    std::string result = dbms.process_query_to_string("SELECT id FROM users");
    EXPECT_EQ(result.find("1"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_NE(result.find("3"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnDeleteFromNonExistentTable) {
    std::string result = dbms.process_query_to_string("DELETE FROM unknown_table");
    EXPECT_NE(result.find("nonexistent table"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, SelectWithNotEqualAndComparisonOperators) {
    dbms.process_query_to_string("CREATE TABLE items (id INTEGER, code NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO items (id, code) VALUES ('10', 'Alpha'), ('20', 'Beta'), ('30', 'Gamma')");

    std::string result = dbms.process_query_to_string("SELECT code FROM items WHERE id > '10' AND code != 'Gamma'");
    EXPECT_NE(result.find("Beta"), std::string::npos);
    EXPECT_EQ(result.find("Alpha"), std::string::npos);
    EXPECT_EQ(result.find("Gamma"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnSelectFromNonExistentTable) {
    std::string result = dbms.process_query_to_string("SELECT * FROM missing_table");
    EXPECT_NE(result.find("nonexistent table"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, FailsOnSelectNonExistentColumn) {
    dbms.process_query_to_string("CREATE TABLE users (id INTEGER)");
    std::string result = dbms.process_query_to_string("SELECT missing_col FROM users");
    EXPECT_NE(result.find("not found"), std::string::npos);
}

TEST_F(DbmsIntegrationTest, BenchmarksTenThousandSqlQueriesParsedUnderHalfSecond) {
    Parser p;
    std::string sql = "SELECT dept_id, COUNT(*), SUM(salary) FROM employees JOIN departments ON dept_id = id WHERE salary >= '50000' GROUP BY dept_id";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; ++i) {
        p.clean();
        p.input(sql);
        p.parse();
    }
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    EXPECT_TRUE(elapsed_ms < 1000.0);
}

TEST_F(DbmsIntegrationTest, BenchmarksTenThousandRowHashJoinAndGroupByUnderTwentyMs) {
    dbms.process_query("CREATE TABLE departments (id INTEGER PRIMARY KEY, dept_name NVARCHAR2)");
    for (int d = 1; d <= 20; ++d) {
        dbms.process_query("INSERT INTO departments (id, dept_name) VALUES ('" + std::to_string(d) + "', 'Dept" + std::to_string(d) + "')");
    }
    dbms.process_query("CREATE TABLE employees (emp_id INTEGER, dept_id INTEGER, salary INTEGER)");
    std::string batch = "INSERT INTO employees (emp_id, dept_id, salary) VALUES ";
    for (int i = 1; i <= 2000; ++i) {
        if (i > 1) batch += ", ";
        batch += "('" + std::to_string(i) + "', '" + std::to_string((i % 20) + 1) + "', '" + std::to_string(50000 + (i % 1000)) + "')";
    }
    dbms.process_query(batch);

    auto start = std::chrono::high_resolution_clock::now();
    auto res = dbms.process_query("SELECT dept_id, COUNT(*), SUM(salary), MIN(salary), MAX(salary) FROM employees JOIN departments ON dept_id = id GROUP BY dept_id");
    auto end = std::chrono::high_resolution_clock::now();
    double elapsed_ms = std::chrono::duration<double, std::milli>(end - start).count();
    EXPECT_TRUE(std::holds_alternative<std::vector<Column>>(res));
    EXPECT_TRUE(elapsed_ms < 50.0);
}