#include "ui.hpp"
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QWidget>

Ui::Ui(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    queryEdit = new QTextEdit(centralWidget);
    QHBoxLayout *buttonsLayout = new QHBoxLayout;
    runButton = new QPushButton("Run Query", centralWidget);
    listTablesButton = new QPushButton("List Tables", centralWidget);
    helpButton = new QPushButton("Help", centralWidget);
    loadButton = new QPushButton("Load", centralWidget);
    buttonsLayout->addWidget(runButton);
    buttonsLayout->addWidget(listTablesButton);
    buttonsLayout->addWidget(helpButton);
    buttonsLayout->addWidget(loadButton);
    outputEdit = new QTextEdit(centralWidget);
    outputEdit->setReadOnly(true);
    mainLayout->addWidget(queryEdit);
    mainLayout->addLayout(buttonsLayout);
    mainLayout->addWidget(outputEdit);
    connect(runButton, &QPushButton::clicked, this, &Ui::onRunButtonClicked);
    connect(listTablesButton, &QPushButton::clicked, this, &Ui::onListTablesClicked);
    connect(helpButton, &QPushButton::clicked, this, &Ui::onHelpButtonClicked);
    connect(loadButton, &QPushButton::clicked, this, &Ui::onLoadButtonClicked);
}

Ui::~Ui() {}

void Ui::onRunButtonClicked() {
    const QString query = queryEdit->toPlainText();
    const std::string result = dbms.process_query(query.toStdString());
    outputEdit->setPlainText(QString::fromStdString(result));
}

void Ui::onListTablesClicked() {
    const std::string result = dbms.process_query("-tables");
    outputEdit->setPlainText(QString::fromStdString(result));
}

void Ui::onHelpButtonClicked() {
    const std::string result = dbms.process_query("-help");
    outputEdit->setPlainText(QString::fromStdString(result));
}

void Ui::onLoadButtonClicked() {
    const std::string result = dbms.process_query("-load");
    outputEdit->setPlainText(QString::fromStdString(result));
}
