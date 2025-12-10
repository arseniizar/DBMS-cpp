#include "ui.hpp"
#include "codeeditor.hpp"

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
#include <QFileDialog>
#include <QIcon>
#include <QKeySequence>
#include <QPalette>
#include <QColor>
#include <QTreeView>
#include <QStandardItemModel>
#include <variant>
#include <QPainter>
#include <QCompleter>
#include <QStringListModel>
#include <QAbstractItemView>
#include <QScrollBar>


Ui::Ui(QWidget* parent)
    : QMainWindow(parent), model(nullptr) {
    setupCompleter();
    setupUi();
    applyDarkTheme();

    connect(tabWidget, &QTabWidget::currentChanged, this, &Ui::updateCompleterContext);

    dbms.load_save();
    updateDatabaseExplorer();
}

Ui::~Ui() {
    delete model;
}

void Ui::setupUi() {
    setWindowTitle("Arsenii's DBMS");
    setWindowIcon(QIcon(":/open.png"));
    resize(1200, 800);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, [this](int index) {
        if (tabWidget->count() > 1) {
            tabWidget->widget(index)->deleteLater();
            tabWidget->removeTab(index);
        }
    });
    setCentralWidget(tabWidget);

    onNewQueryTab();

    createDocks();
    statusBar()->showMessage("Ready", 3000);

    connect(tabWidget, &QTabWidget::currentChanged, this, [this](int index){
       if (const auto* editor = currentQueryEdit()) {
           connect(editor, &CodeEditor::cursorPositionChanged, this, &Ui::updateCompleterContext);
       }
   });
}

void Ui::setupCompleter() {
    completer = new QCompleter(this);
    completerModel = new QStringListModel(this);
    completer->setModel(completerModel);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);

    completer->popup()->setStyleSheet(R"(
        QListView {
            background-color: #252526;
            color: #CCCCCC;
            border: 1px solid #3E3E42;
        }
        QListView::item:selected {
            background-color: #094771;
        }
    )");

    QStringList keywords;
    keywords << "SELECT" << "FROM" << "WHERE" << "INSERT" << "INTO" << "VALUES"
        << "UPDATE" << "SET" << "DELETE" << "CREATE" << "TABLE" << "DROP"
        << "GROUP BY" << "HAVING" << "COUNT" << "PRIMARY KEY" << "FOREIGN KEY"
        << "REFERENCES" << "NVARCHAR2" << "INTEGER" << "DATE";
    completerModel->setStringList(keywords);
}

void Ui::applyDarkTheme() {
    qApp->setStyle("Fusion");

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    darkPalette.setColor(QPalette::Disabled, QPalette::Text, QColor(127, 127, 127));
    darkPalette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(127, 127, 127));

    qApp->setPalette(darkPalette);
}

QIcon create_themed_icon(const QString& resource_path) {
    QIcon icon(resource_path);
    if (icon.isNull()) {
        return QIcon();
    }
    QPixmap pixmap = icon.pixmap(QSize(24, 24));
    QPainter painter(&pixmap);
    painter.setCompositionMode(QPainter::CompositionMode_SourceIn);
    painter.fillRect(pixmap.rect(), qApp->palette().color(QPalette::Active, QPalette::WindowText));
    return QIcon(pixmap);
}

void Ui::createActions() {
    QString runIconPath = ":/run";
    if (!QFile::exists(runIconPath)) {
        qDebug() << "!!! FATAL: Resource file not found at path:" << runIconPath;
    }

    newQueryAction = new QAction(create_themed_icon(":/new"), "&New Query Tab", this);
    newQueryAction->setShortcut(QKeySequence::New);
    connect(newQueryAction, &QAction::triggered, this, &Ui::onNewQueryTab);

    openDbAction = new QAction(create_themed_icon(":/open"), "&Open Database...", this);
    openDbAction->setShortcut(QKeySequence::Open);
    connect(openDbAction, &QAction::triggered, this, &Ui::onOpenDatabase);

    runQueryAction = new QAction(create_themed_icon(":/run"), "&Run Query", this);
    runQueryAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(runQueryAction, &QAction::triggered, this, &Ui::onRunQuery);

    helpAction = new QAction(QIcon::fromTheme("help-contents"), "&Help", this);
    connect(helpAction, &QAction::triggered, this, &Ui::onHelp);

    exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    aboutAction = new QAction(create_themed_icon(":/about"), "&About", this);
    connect(aboutAction, &QAction::triggered, this, &Ui::about);
}

void Ui::createMenus() {
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(newQueryAction);
    fileMenu->addAction(openDbAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu* queryMenu = menuBar()->addMenu("&Query");
    queryMenu->addAction(runQueryAction);

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(helpAction);
    helpMenu->addAction(aboutAction);
}

void Ui::createToolBars() {
    QToolBar* mainToolBar = addToolBar("Main");
    mainToolBar->addAction(newQueryAction);
    mainToolBar->addAction(openDbAction);
    mainToolBar->addAction(runQueryAction);
}

void Ui::createStatusBar() {
    statusBar();
    statusBar()->showMessage("Ready");
}

void Ui::createDocks() {
    QDockWidget* dbExplorerDock = new QDockWidget("Database Explorer", this);
    dbExplorerDock->setStyleSheet(R"(
        QDockWidget::title {
            background-color: #3E3E42;
            text-align: left;
            padding: 6px;
            font-size: 16px;
            font-weight: bold;
        }
    )");

    addDockWidget(Qt::LeftDockWidgetArea, dbExplorerDock);

    dbExplorerView = new QTreeView(dbExplorerDock);
    dbExplorerModel = new QStandardItemModel(dbExplorerDock);
    dbExplorerView->setModel(dbExplorerModel);
    dbExplorerView->setHeaderHidden(true);
    connect(dbExplorerView, &QTreeView::clicked, this, &Ui::onTableClicked);

    dbExplorerView->setStyleSheet(R"(
        QTreeView {
            background-color: #252526; /* Темний фон */
            color: #CCCCCC; /* Світлий текст */
            border: none; /* Без рамки */
            font-size: 14px;
        }
        QTreeView::item {
            padding: 6px; /* Відступи для кожного елемента */
        }
        QTreeView::item:hover {
            background-color: #3E3E42; /* Підсвітка при наведенні */
        }
        QTreeView::item:selected {
            background-color: #094771; /* Колір виділення */
        }
    )");

    dbExplorerDock->setWidget(dbExplorerView);

    QDockWidget* outputDock = new QDockWidget("Output", this);
    addDockWidget(Qt::BottomDockWidgetArea, outputDock);

    QWidget* outputContainer = new QWidget();
    auto* outputLayout = new QVBoxLayout(outputContainer);
    outputLayout->setContentsMargins(0, 0, 0, 0);

    tableView = new QTableView();
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setAlternatingRowColors(true);

    messageEdit = new QTextEdit();
    messageEdit->setReadOnly(true);
    messageEdit->setMaximumHeight(120);

    outputLayout->addWidget(tableView);
    outputLayout->addWidget(messageEdit);

    outputDock->setWidget(outputContainer);

    tableView->setVisible(false);
    messageEdit->setVisible(true);
    messageEdit->setText("Welcome! Open a database or create a new one to begin.");
}

void Ui::updateDatabaseExplorer() {
    dbExplorerModel->clear();
    QStandardItem* root = dbExplorerModel->invisibleRootItem();

    std::vector<std::string> tableNames = dbms.get_table_names();

    QStringList currentList = completerModel->stringList();
    QStringList keywords = {
        "SELECT", "FROM", "WHERE", "INSERT", "INTO", "VALUES", "UPDATE", "SET", "DELETE", "CREATE", "TABLE", "DROP",
        "GROUP BY", "HAVING", "COUNT", "PRIMARY KEY", "FOREIGN KEY", "REFERENCES", "NVARCHAR2", "INTEGER", "DATE"
    };

    for (const auto& tableName : tableNames) {
        keywords.append(QString::fromStdString(tableName));
    }
    completerModel->setStringList(keywords);

    for (const auto& tableName : tableNames) {
        QStandardItem* tableItem = new QStandardItem(QString::fromStdString(tableName));
        tableItem->setIcon(QIcon::fromTheme("database"));
        tableItem->setEditable(false);
        root->appendRow(tableItem);
    }
}

CodeEditor* Ui::currentQueryEdit() {
    return qobject_cast<CodeEditor*>(tabWidget->currentWidget());
}

void Ui::onNewQueryTab() {
    CodeEditor* newQueryEdit = new CodeEditor();
    new SqlHighlighter(newQueryEdit->document());
    newQueryEdit->setCompleter(completer);

    connect(newQueryEdit, &CodeEditor::cursorPositionChanged, this, &Ui::updateCompleterContext);

    int newIndex = tabWidget->addTab(newQueryEdit, QString("Query %1").arg(tabWidget->count() + 1));
    tabWidget->setCurrentIndex(newIndex);
    newQueryEdit->setFocus();
}

void Ui::onTableClicked(const QModelIndex& index) {
    if (!index.parent().isValid()) {
        QString tableName = dbExplorerModel->data(index).toString();
        onNewQueryTab();
        currentQueryEdit()->setText(QString("SELECT * FROM %1;").arg(tableName));

        runQueryAction->trigger();
    }
}

void Ui::onOpenDatabase() {
    QString dir = QFileDialog::getExistingDirectory(this, "Open Database Directory", "../engine/saves/tables");

    if (dir.isEmpty()) return;

    if (dbms.load_database_from_path(dir.toStdString())) {
        statusBar()->showMessage("Database loaded successfully!", 4000);
        updateDatabaseExplorer();
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to load database from the selected directory.");
        statusBar()->showMessage("Failed to load database.", 4000);
    }
}

void Ui::onRunQuery() {
    CodeEditor* currentEdit = currentQueryEdit();
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
        }
        else if constexpr (std::is_same_v<T, std::vector<Column>>) {
            if (arg.empty() || (arg.begin()->get_rows().empty())) {
                messageEdit->setPlainText("Query executed successfully, no rows returned.");
                tableView->setVisible(false);
                messageEdit->setVisible(true);
                statusBar()->showMessage("Query finished. No rows returned.", 3000);
            }
            else {
                model = new SqlTableModel(arg, this);
                tableView->setModel(model);
                messageEdit->setVisible(false);
                tableView->setVisible(true);
                statusBar()->showMessage(QString("Query finished. %1 rows returned.").arg(model->rowCount()), 3000);
            }
        }
    }, result);

    QString upperQuery = query.trimmed().toUpper();
    if (upperQuery.startsWith("CREATE") || upperQuery.startsWith("DROP")) {
        updateDatabaseExplorer();
    }
}

void Ui::onHelp() {
    const std::string result = dbms.process_query_to_string("-help");
    messageEdit->setPlainText(QString::fromStdString(result));
    tableView->setVisible(false);
    messageEdit->setVisible(true);
}

void Ui::about() {
    QMessageBox::about(this, "About Arsenii's DBMS",
                       "A simple yet powerful SQL database management system built with C++ and Qt.\n\n"
                       "Created by Arsenii Zarudniuk.");
}

void Ui::updateCompleterContext() {
    const CodeEditor* editor = currentQueryEdit();
    if (!editor) return;

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor, 2);
    cursor.select(QTextCursor::WordUnderCursor);
    QString prevWord = cursor.selectedText().toUpper();

    QStringList keywords;
    QStringList tableNames;
    for(const auto& name : dbms.get_table_names()){
        tableNames.append(QString::fromStdString(name));
    }

    if (prevWord == "FROM" || prevWord == "TABLE" || prevWord == "INTO" || prevWord == "UPDATE") {
        keywords = tableNames;
    } else {
        keywords << "SELECT" << "FROM" << "WHERE" << "INSERT" << "INTO" << "VALUES"
                 << "UPDATE" << "SET" << "DELETE" << "CREATE" << "TABLE" << "DROP"
                 << "GROUP BY" << "HAVING" << "COUNT";
        keywords.append(tableNames);
    }

    completerModel->setStringList(keywords);
}