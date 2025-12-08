//
// Created by Arsenii Zarudniuk on 08/12/2025.
//

#include "gtest/gtest.h"
#include "Dbms/Dbms.hpp"

class DbmsHavingTest : public ::testing::Test {
protected:
    Dbms dbms;
};

TEST_F(DbmsHavingTest, HandlesHavingWithCountAndGreaterThan) {
    dbms.process_query("CREATE TABLE employees (department NVARCHAR2, salary INTEGER)");
    dbms.process_query("INSERT INTO employees (department, salary) VALUES ('IT', '1000')");
    dbms.process_query("INSERT INTO employees (department, salary) VALUES ('HR', '500')");
    dbms.process_query("INSERT INTO employees (department, salary) VALUES ('IT', '1200')");
    dbms.process_query("INSERT INTO employees (department, salary) VALUES ('IT', '1500')");
    dbms.process_query("INSERT INTO employees (department, salary) VALUES ('HR', '600')");
    dbms.process_query("INSERT INTO employees (department, salary) VALUES ('FINANCE', '800')");

    std::string result = dbms.process_query(
        "SELECT department, COUNT(*) FROM employees GROUP BY department HAVING COUNT(*) > 1"
    );

    EXPECT_NE(result.find("IT"), std::string::npos);
    EXPECT_NE(result.find("3"), std::string::npos);
    EXPECT_NE(result.find("HR"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_EQ(result.find("FINANCE"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingWithCountAndEqualTo) {
    dbms.process_query("CREATE TABLE orders (user_id INTEGER, product NVARCHAR2)");
    dbms.process_query("INSERT INTO orders (user_id, product) VALUES ('1', 'Laptop')");
    dbms.process_query("INSERT INTO orders (user_id, product) VALUES ('2', 'Mouse')");
    dbms.process_query("INSERT INTO orders (user_id, product) VALUES ('1', 'Keyboard')");
    dbms.process_query("INSERT INTO orders (user_id, product) VALUES ('3', 'Monitor')");

    std::string result = dbms.process_query(
        "SELECT user_id, COUNT(*) FROM orders GROUP BY user_id HAVING COUNT(*) = 2"
    );

    EXPECT_NE(result.find("1"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_EQ(result.find("3"), std::string::npos);
}

TEST_F(DbmsHavingTest, RejectsHavingWithoutGroupBy) {
    dbms.process_query("CREATE TABLE data (value INTEGER)");
    dbms.process_query("INSERT INTO data (value) VALUES ('10')");

    std::string result = dbms.process_query("SELECT COUNT(*) FROM data HAVING COUNT(*) > 0");

    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    EXPECT_NE(result.find("error"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingWithGroupByColumn) {
    dbms.process_query("CREATE TABLE cities (name NVARCHAR2, population INTEGER)");
    dbms.process_query("INSERT INTO cities (name, population) VALUES ('Kyiv', '2800')");
    dbms.process_query("INSERT INTO cities (name, population) VALUES ('Lviv', '700')");
    dbms.process_query("INSERT INTO cities (name, population) VALUES ('Kharkiv', '1400')");

    std::string result = dbms.process_query(
        "SELECT name FROM cities GROUP BY name HAVING name = 'Kyiv'"
    );

    EXPECT_NE(result.find("Kyiv"), std::string::npos);
    EXPECT_EQ(result.find("Lviv"), std::string::npos);
    EXPECT_EQ(result.find("Kharkiv"), std::string::npos);
}
