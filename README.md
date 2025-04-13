# Simple DBMS in C++

A basic database management system implemented in C++ that supports essential SQL-like operations. This lightweight engine processes common SQL commands in DDL, DML, and DQL categories. **This is a console application** that allows users to interact directly via the command line.

## Features

- **DDL (Data Definition Language)**
  - `DROP`: Remove a table from the database.
- **DML (Data Manipulation Language)**
  - `INSERT`: Add new records to a table.
  - `UPDATE`: Modify existing records.
- **DQL (Data Query Language)**
  - `SELECT`: Retrieve data from one or more tables.
  - `WHERE`: Filter records based on specific conditions.
  - `GROUP BY`: Aggregate data based on one or more columns.

> **Note:** Some aspects such as performance and full SQL compatibility are not fully confirmed and remain a work in progress.

## Prerequisites

- **C++ Compiler:** A compiler supporting C++17 (e.g., GCC 7+, Clang 5+, MSVC 2017+).
- **Build Tools:** 
  - Optionally [CMake](https://cmake.org/cmake/help/latest/) for an organized build process.
- **Standard Libraries:** Uses only the C++ Standard Library.
- **Additional Libraries:**
  - [fmt](https://fmt.dev/latest/index.html) for formatting output. Ensure that the fmt library is available on your system.

## Installation

Clone the repository from GitHub:

```bash
    git clone https://github.com/arseniizar/DBMS-cpp.git
    cd DBMS-cpp