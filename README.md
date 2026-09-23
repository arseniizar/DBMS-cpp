# Relational DBMS & SQL Execution Engine

![C++20](https://img.shields.io/badge/C%2B%2B-20-blue.svg)
![Qt](https://img.shields.io/badge/Qt-6.x-green.svg)
![Build](https://img.shields.io/badge/CMake-3.20+-orange.svg)
![Tests](https://img.shields.io/badge/Tests-GoogleTest-red.svg)

An educational Relational Database Management System (RDBMS) built from scratch in modern **C++20** with a desktop IDE powered by **Qt 6**. 

The goal of this project is to explore database internals "under the hood": query tokenization, recursive-descent syntax parsing, abstract execution planning, and memory-mapped tabular data structures.

---

## Architecture Overview

The system is decoupled into modular layers:

- **Input:** SQL Query string
- **Lexer & Parser:** Recursive-descent parser and state machine validating syntax and tokens
- **Query Representation:** AST object representing DDL, DML, or DQL commands
- **Execution Engine:** Interprets query plan, performs filtering (`WHERE`), joins, and aggregations
- **Storage Layer:** File-based data storage and type management (`INTEGER`, `NVARCHAR2`, `DATE`)
- **Desktop UI:** Qt 6 interface with `QSyntaxHighlighter` and `QAbstractTableModel`

---

## Key Features

- **Custom SQL Engine:**
  - **DDL:** `CREATE TABLE` (with `PRIMARY KEY`, `FOREIGN KEY`, `REFERENCES`), `DROP TABLE`.
  - **DML:** `INSERT INTO`, `UPDATE`, `DELETE`.
  - **DQL:** `SELECT ... FROM ... WHERE` with multi-conditional filtering (`AND`, comparison operators).
  - **Aggregations & Grouping:** `GROUP BY`, `HAVING`, and `COUNT(...)`.
  - **Relationships:** Basic `JOIN` evaluation between tables.
- **Desktop Database Explorer (Qt 6):**
  - Custom code editor with real-time SQL syntax highlighting (`QSyntaxHighlighter`).
  - High-performance tabular data visualization implemented via `QAbstractTableModel`.
  - Tabbed query workspace and interactive schema explorer.
- **Robust Verification:**
  - Automated unit and integration test suite using **GoogleTest** covering parser state transitions, edge-case query parsing, and execution results.

---

## Screenshot

![App Screenshot](assets/app.png)

---

## Tech Stack & Prerequisites

- **Language:** C++20 (GCC 11+, Clang 13+, or MSVC 2019+)
- **GUI Framework:** Qt 6.x (Core, Gui, Widgets)
- **Build System:** CMake 3.20+
- **Third-Party Libraries:**
  - `fmtlib` for modern string formatting
  - `googletest` for test automation

---

## Building and Running

### 1. Clone the repository
```bash
git clone https://github.com/arseniizar/DBMS-cpp.git
cd DBMS-cpp
```

### 2. Build via CMake
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```

### 3. Run the Application
```bash
./build/DatabaseProject
```

### 4. Run Unit Tests (GoogleTest)
```bash
cd build && ctest --output-on-failure
```
