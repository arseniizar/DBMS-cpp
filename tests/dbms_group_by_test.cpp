//
// Created by Arsenii Zarudniuk on 08/12/2025.
//

#include <gtest/gtest.h>
#include "engine/Dbms/Dbms.hpp"

class DbmsGroupByTest : public ::testing::Test {
protected:
    Dbms dbms;
};

TEST_F(DbmsGroupByTest, HandlesSimpleGroupBy) {
    dbms.process_query("CREATE TABLE sales (city NVARCHAR2, amount INTEGER)");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Kyiv', '100')");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Lviv', '200')");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Kyiv', '50')");

    std::string result = dbms.process_query_to_string("SELECT city FROM sales GROUP BY city");

    EXPECT_NE(result.find("Kyiv"), std::string::npos);
    EXPECT_NE(result.find("Lviv"), std::string::npos);
    EXPECT_EQ(result.find("Kyiv", result.find("Kyiv") + 1), std::string::npos);
}

TEST_F(DbmsGroupByTest, HandlesGroupByWithCountStar) {
    dbms.process_query("CREATE TABLE employees (department NVARCHAR2, name NVARCHAR2)");
    dbms.process_query("INSERT INTO employees (department, name) VALUES ('IT', 'Arsenii')");
    dbms.process_query("INSERT INTO employees (department, name) VALUES ('HR', 'Olena')");
    dbms.process_query("INSERT INTO employees (department, name) VALUES ('IT', 'Max')");
    dbms.process_query("INSERT INTO employees (department, name) VALUES ('IT', 'Yana')");
    dbms.process_query("INSERT INTO employees (department, name) VALUES ('HR', 'Ivan')");

    std::string result = dbms.process_query_to_string("SELECT department, COUNT(*) FROM employees GROUP BY department");

    EXPECT_NE(result.find("IT"), std::string::npos);
    EXPECT_NE(result.find("3"), std::string::npos);
    EXPECT_NE(result.find("HR"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
}

TEST_F(DbmsGroupByTest, HandlesGroupByWithCountColumn) {
    dbms.process_query("CREATE TABLE visits (city NVARCHAR2, browser NVARCHAR2)");
    dbms.process_query("INSERT INTO visits (city, browser) VALUES ('Kyiv', 'Chrome')");
    dbms.process_query("INSERT INTO visits (city, browser) VALUES ('Lviv', 'Firefox')");
    dbms.process_query("INSERT INTO visits (city, browser) VALUES ('Kyiv', 'Chrome')");
    dbms.process_query("INSERT INTO visits (city, browser) VALUES ('Kyiv', '')");

    std::string result = dbms.process_query_to_string("SELECT city, COUNT(browser) FROM visits GROUP BY city");

    EXPECT_NE(result.find("Kyiv"), std::string::npos);
    EXPECT_NE(result.find("3"), std::string::npos);
    EXPECT_NE(result.find("Lviv"), std::string::npos);
    EXPECT_NE(result.find("1"), std::string::npos);
}

TEST_F(DbmsGroupByTest, HandlesGroupByWithSumMinMax) {
    dbms.process_query("CREATE TABLE sales (city NVARCHAR2, amount INTEGER)");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Kyiv', '100')");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Kyiv', '250')");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Lviv', '80')");

    std::string result = dbms.process_query_to_string(
        "SELECT city, SUM(amount), MIN(amount), MAX(amount) FROM sales GROUP BY city");

    EXPECT_NE(result.find("Kyiv"), std::string::npos);
    EXPECT_NE(result.find("350"), std::string::npos);
    EXPECT_NE(result.find("100"), std::string::npos);
    EXPECT_NE(result.find("250"), std::string::npos);
    EXPECT_NE(result.find("Lviv"), std::string::npos);
    EXPECT_NE(result.find("80"), std::string::npos);
}

TEST_F(DbmsGroupByTest, HandlesGroupByWithSumOnly) {
    dbms.process_query("CREATE TABLE orders (dept NVARCHAR2, cost INTEGER)");
    dbms.process_query("INSERT INTO orders (dept, cost) VALUES ('Eng', '400')");
    dbms.process_query("INSERT INTO orders (dept, cost) VALUES ('Eng', '600')");
    dbms.process_query("INSERT INTO orders (dept, cost) VALUES ('Sales', '300')");

    std::string result = dbms.process_query_to_string("SELECT dept, SUM(cost) FROM orders GROUP BY dept");
    EXPECT_NE(result.find("1000"), std::string::npos);
    EXPECT_NE(result.find("300"), std::string::npos);
}

TEST_F(DbmsGroupByTest, HandlesGroupByWithMinOnly) {
    dbms.process_query("CREATE TABLE scores (team NVARCHAR2, points INTEGER)");
    dbms.process_query("INSERT INTO scores (team, points) VALUES ('Alpha', '45')");
    dbms.process_query("INSERT INTO scores (team, points) VALUES ('Alpha', '15')");
    dbms.process_query("INSERT INTO scores (team, points) VALUES ('Beta', '30')");

    std::string result = dbms.process_query_to_string("SELECT team, MIN(points) FROM scores GROUP BY team");
    EXPECT_NE(result.find("15"), std::string::npos);
    EXPECT_NE(result.find("30"), std::string::npos);
}

TEST_F(DbmsGroupByTest, HandlesGroupByWithMaxOnly) {
    dbms.process_query("CREATE TABLE scores (team NVARCHAR2, points INTEGER)");
    dbms.process_query("INSERT INTO scores (team, points) VALUES ('Alpha', '45')");
    dbms.process_query("INSERT INTO scores (team, points) VALUES ('Alpha', '95')");
    dbms.process_query("INSERT INTO scores (team, points) VALUES ('Beta', '60')");

    std::string result = dbms.process_query_to_string("SELECT team, MAX(points) FROM scores GROUP BY team");
    EXPECT_NE(result.find("95"), std::string::npos);
    EXPECT_NE(result.find("60"), std::string::npos);
}

TEST_F(DbmsGroupByTest, HandlesGroupByFilteredByWhere) {
    dbms.process_query("CREATE TABLE employees (dept NVARCHAR2, status NVARCHAR2, salary INTEGER)");
    dbms.process_query("INSERT INTO employees (dept, status, salary) VALUES ('IT', 'active', '5000')");
    dbms.process_query("INSERT INTO employees (dept, status, salary) VALUES ('IT', 'inactive', '4000')");
    dbms.process_query("INSERT INTO employees (dept, status, salary) VALUES ('IT', 'active', '6000')");

    std::string result = dbms.process_query_to_string(
        "SELECT dept, SUM(salary) FROM employees WHERE status = 'active' GROUP BY dept");
    EXPECT_NE(result.find("11000"), std::string::npos);
}

TEST_F(DbmsGroupByTest, HandlesGroupByOnIntegerColumn) {
    dbms.process_query("CREATE TABLE logs (code INTEGER, latency INTEGER)");
    dbms.process_query("INSERT INTO logs (code, latency) VALUES ('200', '10')");
    dbms.process_query("INSERT INTO logs (code, latency) VALUES ('200', '20')");
    dbms.process_query("INSERT INTO logs (code, latency) VALUES ('500', '100')");

    std::string result = dbms.process_query_to_string("SELECT code, COUNT(*) FROM logs GROUP BY code");
    EXPECT_NE(result.find("200"), std::string::npos);
    EXPECT_NE(result.find("2"), std::string::npos);
    EXPECT_NE(result.find("500"), std::string::npos);
}