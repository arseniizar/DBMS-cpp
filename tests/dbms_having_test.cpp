//
// Created by Arsenii Zarudniuk on 08/12/2025.
//

#include "gtest/gtest.h"
#include "engine/Dbms/Dbms.hpp"

class DbmsHavingTest : public ::testing::Test {
protected:
    Dbms dbms;
};

TEST_F(DbmsHavingTest, HandlesHavingWithCountAndGreaterThan) {
    dbms.process_query_to_string("CREATE TABLE employees (department NVARCHAR2, salary INTEGER)");
    dbms.process_query_to_string("INSERT INTO employees (department, salary) VALUES ('IT', '1000')");
    dbms.process_query_to_string("INSERT INTO employees (department, salary) VALUES ('HR', '500')");
    dbms.process_query_to_string("INSERT INTO employees (department, salary) VALUES ('IT', '1200')");
    dbms.process_query_to_string("INSERT INTO employees (department, salary) VALUES ('IT', '1500')");
    dbms.process_query_to_string("INSERT INTO employees (department, salary) VALUES ('HR', '600')");
    dbms.process_query_to_string("INSERT INTO employees (department, salary) VALUES ('FINANCE', '800')");

    std::string result = dbms.process_query_to_string(
        "SELECT department, COUNT(*) FROM employees GROUP BY department HAVING COUNT(*) > 1"
    );

    EXPECT_NE(result.find("IT"), std::string::npos);
    EXPECT_NE(result.find("3"), std::string::npos);
    EXPECT_NE(result.find("HR"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_EQ(result.find("FINANCE"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingWithCountAndEqualTo) {
    dbms.process_query_to_string("CREATE TABLE orders (user_id INTEGER, product NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO orders (user_id, product) VALUES ('1', 'Laptop')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, product) VALUES ('2', 'Mouse')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, product) VALUES ('1', 'Keyboard')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, product) VALUES ('3', 'Monitor')");

    std::string result = dbms.process_query_to_string(
        "SELECT user_id, COUNT(*) FROM orders GROUP BY user_id HAVING COUNT(*) = 2"
    );

    EXPECT_NE(result.find("1"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_EQ(result.find("3"), std::string::npos);
}

TEST_F(DbmsHavingTest, RejectsHavingWithoutGroupBy) {
    dbms.process_query_to_string("CREATE TABLE data (value INTEGER)");
    dbms.process_query_to_string("INSERT INTO data (value) VALUES ('10')");

    std::string result = dbms.process_query_to_string("SELECT COUNT(*) FROM data HAVING COUNT(*) > 0");

    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    EXPECT_NE(result.find("error"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingWithGroupByColumn) {
    dbms.process_query_to_string("CREATE TABLE cities (name NVARCHAR2, population INTEGER)");
    dbms.process_query_to_string("INSERT INTO cities (name, population) VALUES ('Kyiv', '2800')");
    dbms.process_query_to_string("INSERT INTO cities (name, population) VALUES ('Lviv', '700')");
    dbms.process_query_to_string("INSERT INTO cities (name, population) VALUES ('Kharkiv', '1400')");

    std::string result = dbms.process_query_to_string(
        "SELECT name FROM cities GROUP BY name HAVING name = 'Kyiv'"
    );

    EXPECT_NE(result.find("Kyiv"), std::string::npos);
    EXPECT_EQ(result.find("Lviv"), std::string::npos);
    EXPECT_EQ(result.find("Kharkiv"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingWithLessThan) {
    dbms.process_query_to_string("CREATE TABLE orders (user_id INTEGER, item NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('1', 'Book')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('1', 'Pen')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('2', 'Notebook')");

    std::string result = dbms.process_query_to_string(
        "SELECT user_id, COUNT(*) FROM orders GROUP BY user_id HAVING COUNT(*) < 2"
    );

    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_EQ(result.find("1\t2"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingWithGreaterOrEqual) {
    dbms.process_query_to_string("CREATE TABLE orders (user_id INTEGER, item NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('10', 'Book')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('10', 'Pen')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('20', 'Notebook')");

    std::string result = dbms.process_query_to_string(
        "SELECT user_id, COUNT(*) FROM orders GROUP BY user_id HAVING COUNT(*) >= 2"
    );

    EXPECT_NE(result.find("10"), std::string::npos);
    EXPECT_EQ(result.find("20"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingWithLessOrEqual) {
    dbms.process_query_to_string("CREATE TABLE orders (user_id INTEGER, item NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('10', 'Book')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('10', 'Pen')");
    dbms.process_query_to_string("INSERT INTO orders (user_id, item) VALUES ('20', 'Notebook')");

    std::string result = dbms.process_query_to_string(
        "SELECT user_id, COUNT(*) FROM orders GROUP BY user_id HAVING COUNT(*) <= 1"
    );

    EXPECT_NE(result.find("20"), std::string::npos);
    EXPECT_EQ(result.find("10"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingWithNotEqual) {
    dbms.process_query_to_string("CREATE TABLE cities (name NVARCHAR2, region NVARCHAR2)");
    dbms.process_query_to_string("INSERT INTO cities (name, region) VALUES ('Kyiv', 'North')");
    dbms.process_query_to_string("INSERT INTO cities (name, region) VALUES ('Lviv', 'West')");

    std::string result = dbms.process_query_to_string(
        "SELECT name FROM cities GROUP BY name HAVING name != 'Lviv'"
    );

    EXPECT_NE(result.find("Kyiv"), std::string::npos);
    EXPECT_EQ(result.find("Lviv"), std::string::npos);
}

TEST_F(DbmsHavingTest, HandlesHavingAfterWhereFilter) {
    dbms.process_query_to_string("CREATE TABLE events (city NVARCHAR2, active INTEGER)");
    dbms.process_query_to_string("INSERT INTO events (city, active) VALUES ('Warsaw', '1')");
    dbms.process_query_to_string("INSERT INTO events (city, active) VALUES ('Warsaw', '1')");
    dbms.process_query_to_string("INSERT INTO events (city, active) VALUES ('Kyiv', '1')");
    dbms.process_query_to_string("INSERT INTO events (city, active) VALUES ('Kyiv', '0')");

    std::string result = dbms.process_query_to_string(
        "SELECT city, COUNT(*) FROM events WHERE active = '1' GROUP BY city HAVING COUNT(*) > 1"
    );

    EXPECT_NE(result.find("Warsaw"), std::string::npos);
    EXPECT_EQ(result.find("Kyiv"), std::string::npos);
}
