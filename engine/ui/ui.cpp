#include "ui.hpp"
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QTableView>
#include <QHeaderView>
#include <variant>

Ui::Ui(QWidget *parent)
    : QMainWindow(parent), model(nullptr)
{
    setWindowTitle("Arsenii's DBMS");
    resize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    queryEdit = new QTextEdit(centralWidget);
    queryEdit->setPlaceholderText("Enter your SQL query here...");

    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    runButton = new QPushButton("▶️ Run Query", centralWidget);
    listTablesButton = new QPushButton("List Tables", centralWidget);
    helpButton = new QPushButton("❓ Help", centralWidget);
    loadButton = new QPushButton("💾 Load", centralWidget);

    buttonsLayout->addWidget(runButton);
    buttonsLayout->addWidget(listTablesButton);
    buttonsLayout->addWidget(helpButton);
    buttonsLayout->addWidget(loadButton);
    buttonsLayout->addStretch();

    tableView = new QTableView(centralWidget);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setAlternatingRowColors(true);

    messageEdit = new QTextEdit(centralWidget);
    messageEdit->setReadOnly(true);
    messageEdit->setMaximumHeight(100);

    mainLayout->addWidget(queryEdit, 3);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(tableView, 5);
    mainLayout->addWidget(messageEdit);

    tableView->setVisible(false);
    messageEdit->setVisible(true);
    messageEdit->setText("Welcome to the DBMS! Enter a query to begin.");

    connect(runButton, &QPushButton::clicked, this, &Ui::onRunButtonClicked);
    connect(listTablesButton, &QPushButton::clicked, this, &Ui::onListTablesClicked);
    connect(helpButton, &QPushButton::clicked, this, &Ui::onHelpButtonClicked);
    connect(loadButton, &QPushButton::clicked, this, &Ui::onLoadButtonClicked);
}

Ui::~Ui() {
    delete model;
}

void Ui::onRunButtonClicked() {
    // Очищуємо попередній вивід
    messageEdit->clear();
    if (model) {
        delete model;
        model = nullptr;
    }
    tableView->setModel(nullptr);

    const QString query = queryEdit->toPlainText();
    if (query.trimmed().isEmpty()) {
        messageEdit->setText("Query is empty.");
        return;
    }

    QueryResult result = dbms.process_query(query.toStdString());

    // Обробляємо результат
    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            // Результат - рядок (помилка або повідомлення)
            messageEdit->setPlainText(QString::fromStdString(arg));
            tableView->setVisible(false);
            messageEdit->setVisible(true);
        } else if constexpr (std::is_same_v<T, std::vector<Column>>) {
            // Результат - вектор колонок
            if (arg.empty() || arg[0].get_rows().empty()) {
                messageEdit->setPlainText("Query executed successfully, no rows returned.");
                tableView->setVisible(false);
                messageEdit->setVisible(true);
            } else {
                model = new SqlTableModel(arg, this);
                tableView->setModel(model);
                messageEdit->setVisible(false);
                tableView->setVisible(true);
            }
        }
    }, result);
}

void Ui::onListTablesClicked() {
    std::string result = dbms.process_query_to_string("-tables");
    messageEdit->setPlainText(QString::fromStdString(result));
    tableView->setVisible(false);
    messageEdit->setVisible(true);
}

void Ui::onHelpButtonClicked() {
    std::string result = dbms.process_query_to_string("-help");
    messageEdit->setPlainText(QString::fromStdString(result));
    tableView->setVisible(false);
    messageEdit->setVisible(true);
}

void Ui::onLoadButtonClicked() {
    std::string result = dbms.process_query_to_string("-load");
    messageEdit->setPlainText(QString::fromStdString(result));
    tableView->setVisible(false);
    messageEdit->setVisible(true);
}
