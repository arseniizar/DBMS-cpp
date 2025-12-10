#include "infowidget.hpp"
#include <QTextEdit>
#include <QVBoxLayout>

InfoWidget::InfoWidget(QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    auto* infoTextEdit = new QTextEdit(this);
    infoTextEdit->setReadOnly(true);

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
                    padding: 15px;
                }
                h1 {
                    color: #FFFFFF;
                    border-bottom: 2px solid #FFFFFF;
                    padding-bottom: 10px;
                }
                h2 {
                    color: #FFFFFF;
                    margin-top: 30px;
                }
                p {
                    font-size: 16px;
                }
                ul {
                    list-style-type: square;
                    padding-left: 20px;
                }
                li {
                    margin-bottom: 10px;
                    font-size: 16px;
                }
                code {
                    background-color: #424242;
                    border-radius: 4px;
                    padding: 2px 5px;
                    font-family: 'Courier New', Courier, monospace;
                }
            </style>
        </head>
        <body>
            <h1>About the Project: Arsenii's DBMS</h1>
            <p>
                This is an educational Database Management System (DBMS) project, built from scratch in C++ using the Qt framework for the graphical interface.
                The main goal is to understand how databases work "under the hood."
            </p>

            <h2>Key Features</h2>
            <ul>
                <li>
                    <b>Custom SQL Engine:</b> All queries, from text parsing to execution, are handled by custom-written code.
                    The syntax is partly inspired by PL/SQL.
                </li>
                <li>
                    <b>Graphical Interface:</b> The application features a user-friendly window with a dark theme, query tabs,
                    a table explorer, and SQL syntax highlighting.
                </li>
                <li>
                    <b>Core SQL Commands:</b> Key operations are supported: <code>CREATE TABLE</code>, <code>DROP TABLE</code>,
                    <code>INSERT INTO</code>, <code>SELECT</code> (with <code>WHERE</code>, <code>GROUP BY</code>, <code>HAVING</code>), <code>UPDATE</code>, and <code>DELETE</code>.
                </li>
                <li>
                    <b>Developer Helpers:</b> Built-in code completion for keywords and table names helps to speed up query writing.
                </li>
                <li>
                    <b>Save & Load:</b> The database (tables and their contents) is saved as plain text files in a dedicated folder,
                    making it easy to view and transfer.
                </li>
            </ul>
            <p>
                This project is constantly evolving, with the primary focus on learning and experimentation rather than on creating a competitor to existing DBMS solutions.
            </p>
        </body>
        </html>
    )";

    infoTextEdit->setHtml(htmlContent);
    layout->addWidget(infoTextEdit);
    setLayout(layout);
}
