#include "helpwidget.hpp"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QFile>
#include <QDebug>

HelpWidget::HelpWidget(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* helpTextEdit = new QTextEdit(this);
    helpTextEdit->setReadOnly(true);

    QString htmlContent = R"(
        <!DOCTYPE html>
        <html>
        <head>
            <style>
                body {
                    font-family: Arial, sans-serif;
                    background-color: #2D2D2D;
                    color: #E0E0E0;
                    line-height: 1.6;
                }
                h1 {
                    color: #FFFFFF;
                    border-bottom: 2px solid #FFFFFF;
                    padding-bottom: 10px;
                }
                h2 {
                    color: #FFFFFF;
                    border-bottom: 1px solid #FFFFFF;
                    padding-bottom: 5px;
                    margin-top: 30px;
                }
                code {
                    background-color: #424242;
                    border-radius: 4px;
                    padding: 3px 6px;
                    font-family: 'Courier New', Courier, monospace;
                    font-size: 14px;
                    color: #F8F8F2;
                }
                pre {
                    background-color: #383838;
                    border: 1px solid #555;
                    border-radius: 5px;
                    padding: 15px;
                    white-space: pre-wrap;
                    word-wrap: break-word;
                    font-family: 'Courier New', Courier, monospace;
                    font-size: 14px;
                }
                ul {
                    list-style-type: square;
                    padding-left: 20px;
                }
                li {
                    margin-bottom: 10px;
                }
            </style>
        </head>
        <body>
            <h1>SQL Command Reference</h1>
            <p>Welcome to ADBMS! Below is a list of available SQL commands with examples.</p>

            <h2>CREATE TABLE</h2>
            <p>Creates a new table in the database.</p>
            <p><b>Syntax:</b></p>
            <pre><code>CREATE TABLE <i>table_name</i> (
    <i>column1 datatype1 [constraints]</i>,
    <i>column2 datatype2 [constraints]</i>,
    ...
);</code></pre>
            <p><b>Example:</b></p>
            <pre><code>CREATE TABLE users (
    id INTEGER PRIMARY KEY,
    name NVARCHAR2,
    email NVARCHAR2
);</code></pre>

            <h2>DROP TABLE</h2>
            <p>Deletes an existing table from the database.</p>
            <p><b>Syntax:</b></p>
            <pre><code>DROP TABLE <i>table_name</i>;</code></pre>
            <p><b>Example:</b></p>
            <pre><code>DROP TABLE users;</code></pre>

            <h2>INSERT INTO</h2>
            <p>Adds new rows of data to a table.</p>
            <p><b>Syntax:</b></p>
            <pre><code>INSERT INTO <i>table_name</i> (<i>column1, column2, ...</i>)
VALUES ('<i>value1</i>', '<i>value2</i>', ...);</code></pre>
             <p>Inserting multiple rows at once is also supported:</p>
            <pre><code>INSERT INTO <i>table_name</i> (<i>column1, column2</i>)
VALUES ('<i>value_a1</i>', '<i>value_a2</i>'), ('<i>value_b1</i>', '<i>value_b2</i>');</code></pre>
            <p><b>Example:</b></p>
            <pre><code>INSERT INTO users (id, name, email)
VALUES ('1', 'Arsenii', 'arsenii@example.com'), ('2', 'Olena', 'olena@example.com');</code></pre>

            <h2>SELECT</h2>
            <p>Selects data from one or more tables.</p>
            <p><b>Syntax:</b></p>
            <pre><code>SELECT <i>column1, column2, ...</i>
FROM <i>table_name</i>
[WHERE <i>condition</i>]
[GROUP BY <i>column</i>]
[HAVING <i>condition</i>];</code></pre>
            <p><b>Examples:</b></p>
            <pre><code>-- Select all data from a table
SELECT * FROM users;

-- Select specific columns with a condition
SELECT name, email FROM users WHERE id = '1';

-- Using an aggregate function
SELECT department, COUNT(*) FROM employees GROUP BY department HAVING COUNT(*) > 5;</code></pre>

            <h2>UPDATE</h2>
            <p>Updates existing records in a table.</p>
            <p><b>Syntax:</b></p>
            <pre><code>UPDATE <i>table_name</i>
SET <i>column1</i> = '<i>new_value1</i>', <i>column2</i> = '<i>new_value2</i>', ...
[WHERE <i>condition</i>];</code></pre>
            <p><b>Example:</b></p>
            <pre><code>UPDATE users SET email = 'new.arsenii@example.com' WHERE name = 'Arsenii';</code></pre>

            <h2>DELETE FROM</h2>
            <p>Deletes existing records from a table.</p>
            <p><b>Syntax:</b></p>
            <pre><code>DELETE FROM <i>table_name</i> [WHERE <i>condition</i>];</code></pre>
            <p><b>Example:</b></p>
            <pre><code>DELETE FROM users WHERE id = '2';</code></pre>

            <h2>Utility Commands</h2>
            <ul>
                <li><code>-tables</code>: Show a list of all tables in the database.</li>
                <li><code>-help</code>: Open this help page.</li>
                <li><code>-load</code>: Open the console prompt to load saves.</li>
            </ul>
        </body>
        </html>
    )";

    helpTextEdit->setHtml(htmlContent);
    layout->addWidget(helpTextEdit);
    setLayout(layout);
}
