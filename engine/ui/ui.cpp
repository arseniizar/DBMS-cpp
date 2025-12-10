#include "ui.hpp"

#include <QApplication>
#include <QTabWidget>
#include <QTextEdit>
#include <QTableView>
#include <QStatusBar>
#include <QMenuBar>
#include <QToolBar>
#include <QDockWidget>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QHeaderView>
#include <QKeySequence>
#include <QIcon>
#include <variant>

Ui::Ui(QWidget *parent)
    : QMainWindow(parent), model(nullptr), highlighter(nullptr)
{
    setWindowTitle("Arsenii's DBMS");
    setWindowIcon(QIcon::fromTheme("application-x-sqlite3"));
    resize(1200, 800);

    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    QTextEdit* initialQueryEdit = new QTextEdit();
    highlighter = new SqlHighlighter(initialQueryEdit->document());
    tabWidget->addTab(initialQueryEdit, "Query 1");
    setCentralWidget(tabWidget);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDocks();

    statusBar()->showMessage("Ready", 3000);
}

Ui::~Ui() {
    delete model;
}

void Ui::createActions() {
    runQueryAction = new QAction(QIcon::fromTheme("media-playback-start", QIcon(":/icons/run.png")), "&Run Query", this);
    runQueryAction->setShortcut(QKeySequence(Qt::Key_F5));
    runQueryAction->setStatusTip("Execute the current SQL query");
    connect(runQueryAction, &QAction::triggered, this, &Ui::onRunQuery);

    listTablesAction = new QAction("&List Tables", this);
    connect(listTablesAction, &QAction::triggered, this, &Ui::onListTables);

    helpAction = new QAction(QIcon::fromTheme("help-contents"), "&Help", this);
    connect(helpAction, &QAction::triggered, this, &Ui::onHelp);

    loadAction = new QAction(QIcon::fromTheme("document-open"), "&Load", this);
    connect(loadAction, &QAction::triggered, this, &Ui::onLoad);

    exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    aboutAction = new QAction("&About", this);
    connect(aboutAction, &QAction::triggered, this, &Ui::about);
}

void Ui::createMenus() {
    fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(loadAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    queryMenu = menuBar()->addMenu("&Query");
    queryMenu->addAction(runQueryAction);
    queryMenu->addAction(listTablesAction);

    helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
}

void Ui::createToolBars() {
    mainToolBar = addToolBar("Main");
    mainToolBar->addAction(runQueryAction);
    mainToolBar->addAction(loadAction);
    mainToolBar->addAction(helpAction);
}

void Ui::createStatusBar() {
    statusBar();
    statusBar()->showMessage("Ready");
}

void Ui::createDocks() {
    QDockWidget *outputDock = new QDockWidget("Output", this);
    addDockWidget(Qt::BottomDockWidgetArea, outputDock);

    QWidget *outputContainer = new QWidget();
    QVBoxLayout *outputLayout = new QVBoxLayout(outputContainer);

    tableView = new QTableView();
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setAlternatingRowColors(true);

    messageEdit = new QTextEdit();
    messageEdit->setReadOnly(true);
    messageEdit->setMaximumHeight(120);

    outputLayout->addWidget(tableView);
    outputLayout->addWidget(messageEdit);
    outputContainer->setLayout(outputLayout);

    outputDock->setWidget(outputContainer);

    tableView->setVisible(false);
    messageEdit->setVisible(true);
    messageEdit->setText("Welcome to the DBMS! Enter a query to begin.");
}

QTextEdit* Ui::currentQueryEdit() {
    return qobject_cast<QTextEdit*>(tabWidget->currentWidget());
}


void Ui::onRunQuery() {
    QTextEdit* currentEdit = currentQueryEdit();
    if (!currentEdit) return;

    messageEdit->clear();
    if (model) {
        delete model;
        model = nullptr;
    }
    tableView->setModel(nullptr);

    const QString query = currentEdit->toPlainText();
    if (query.trimmed().isEmpty()) {
        statusBar()->showMessage("Query is empty.", 3000);
        return;
    }

    statusBar()->showMessage("Executing query...");
    QueryResult result = dbms.process_query(query.toStdString());

    std::visit([this](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            messageEdit->setPlainText(QString::fromStdString(arg));
            tableView->setVisible(false);
            messageEdit->setVisible(true);
            statusBar()->showMessage("Query finished.", 3000);
        } else if constexpr (std::is_same_v<T, std::vector<Column>>) {
            if (arg.empty() || arg[0].get_rows().empty()) {
                messageEdit->setPlainText("Query executed successfully, no rows returned.");
                tableView->setVisible(false);
                messageEdit->setVisible(true);
                statusBar()->showMessage("Query finished. No rows returned.", 3000);
            } else {
                model = new SqlTableModel(arg, this);
                tableView->setModel(model);
                messageEdit->setVisible(false);
                tableView->setVisible(true);
                statusBar()->showMessage(QString("Query finished. %1 rows returned.").arg(model->rowCount()), 3000);
            }
        }
    }, result);
}

void Ui::onListTables() {
    std::string result = dbms.process_query_to_string("-tables");
    messageEdit->setPlainText(QString::fromStdString(result));
    tableView->setVisible(false);
    messageEdit->setVisible(true);
}

void Ui::onHelp() {
    std::string result = dbms.process_query_to_string("-help");
    messageEdit->setPlainText(QString::fromStdString(result));
    tableView->setVisible(false);
    messageEdit->setVisible(true);
}

void Ui::onLoad() {
    std::string result = dbms.process_query_to_string("-load");
    messageEdit->setPlainText(QString::fromStdString(result) + "\n(Please check the console for interaction)");
    tableView->setVisible(false);
    messageEdit->setVisible(true);
}

void Ui::about() {
    QMessageBox::about(this, "About Arsenii's DBMS",
        "A simple yet powerful SQL database management system built with C++ and Qt.\n\n"
        "Created by Arsenii Zarudniuk.");
}
