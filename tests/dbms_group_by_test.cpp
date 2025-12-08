//
// Created by Arsenii Zarudniuk on 08/12/2025.
//

#include <gtest/gtest.h>
#include "Dbms/Dbms.hpp"

class DbmsGroupByTest : public ::testing::Test {
protected:
    Dbms dbms;
};

TEST_F(DbmsGroupByTest, HandlesSimpleGroupBy) {
    dbms.process_query("CREATE TABLE sales (city NVARCHAR2, amount INTEGER)");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Kyiv', '100')");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Lviv', '200')");
    dbms.process_query("INSERT INTO sales (city, amount) VALUES ('Kyiv', '50')");

    std::string result = dbms.process_query("SELECT city FROM sales GROUP BY city");

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

    std::string result = dbms.process_query("SELECT department, COUNT(*) FROM employees GROUP BY department");

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

    std::string result = dbms.process_query("SELECT city, COUNT(browser) FROM visits GROUP BY city");

    EXPECT_NE(result.find("Kyiv"), std::string::npos);
    EXPECT_NE(result.find("3"), std::string::npos);
    EXPECT_NE(result.find("Lviv"), std::string::npos);
    EXPECT_NE(result.find("1"), std::string::npos);
}