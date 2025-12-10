#include "ui.hpp"
#include "codeeditor.hpp"
#include "querytab.hpp"
#include "helpwidget.hpp"
#include "infowidget.hpp"

#include <QApplication>
#include <QColor>
#include <QCompleter>
#include <QDebug>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QHeaderView>
#include <QIcon>
#include <QKeySequence>
#include <QMenuBar>
#include <QMessageBox>
#include <QPainter>
#include <QPalette>
#include <QRegularExpression>
#include <QStandardItemModel>
#include <QStatusBar>
#include <QStringListModel>
#include <QTabBar>
#include <QTabWidget>
#include <QTableView>
#include <QTextEdit>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>
#include <variant>

QIcon create_themed_icon(const QString& resource_path)
{
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

Ui::Ui(QWidget* parent) : QMainWindow(parent)
{
    setupUi();
    setupCompleter();
    applyDarkTheme();

    for (int i = 0; i < tabWidget->count(); ++i) {
        if (auto* tab = qobject_cast<QueryTab*>(tabWidget->widget(i))) {
            tab->getEditor()->setCompleter(completer);
        }
    }

    dbms.load_save();
    updateDatabaseExplorer();
}

Ui::~Ui() {}

void Ui::setupUi()
{
    setWindowTitle("DBMS");
    setWindowIcon(QIcon(":/open"));
    resize(1200, 800);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    createDocks();

    tabWidget = new QTabWidget(this);
    tabWidget->setTabsClosable(true);
    connect(tabWidget, &QTabWidget::tabCloseRequested, this, &Ui::closeTab);
    setCentralWidget(tabWidget);

    onNewQueryTab(true);
}

void Ui::setupCompleter()
{
    completer = new QCompleter(this);
    completerModel = new QStringListModel(this);
    completer->setModel(completerModel);
    completer->setModelSorting(QCompleter::CaseInsensitivelySortedModel);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    completer->setWrapAround(false);
    completer->setFilterMode(Qt::MatchContains);

    completer->popup()->setStyleSheet(R"(
        QListView { background-color: #252526; color: #CCCCCC; border: 1px solid #3E3E42; }
        QListView::item:selected { background-color: #094771; }
    )");

    updateCompleterContext();
}

void Ui::applyDarkTheme()
{
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
    qApp->setPalette(darkPalette);
}

void Ui::createActions()
{
    newQueryAction = new QAction(create_themed_icon(":/new"), "&New Query Tab", this);
    newQueryAction->setShortcut(QKeySequence::New);
    connect(newQueryAction, &QAction::triggered, this, [this]() { onNewQueryTab(false); });

    openDbAction = new QAction(create_themed_icon(":/open"), "&Open Database...", this);
    openDbAction->setShortcut(QKeySequence::Open);
    connect(openDbAction, &QAction::triggered, this, &Ui::onOpenDatabase);

    runQueryAction = new QAction(create_themed_icon(":/run"), "&Run Query", this);
    runQueryAction->setShortcut(QKeySequence(Qt::Key_F5));
    connect(runQueryAction, &QAction::triggered, this, &Ui::onRunQuery);

    exitAction = new QAction("E&xit", this);
    exitAction->setShortcut(QKeySequence::Quit);
    connect(exitAction, &QAction::triggered, qApp, &QApplication::quit);

    aboutAction = new QAction(create_themed_icon(":/about"), "&About", this);
    connect(aboutAction, &QAction::triggered, this, &Ui::about);

    helpAction = new QAction(QIcon::fromTheme("help-contents"), "&Help", this);
    connect(helpAction, &QAction::triggered, this, &Ui::onHelp);

    infoAction = new QAction(create_themed_icon(":/about"), "&Info", this);
    connect(infoAction, &QAction::triggered, this, &Ui::onInfo);
}

void Ui::createMenus()
{
    QMenu* fileMenu = menuBar()->addMenu("&File");
    fileMenu->addAction(newQueryAction);
    fileMenu->addAction(openDbAction);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAction);

    QMenu* queryMenu = menuBar()->addMenu("&Query");
    queryMenu->addAction(runQueryAction);

    QMenu* helpMenu = menuBar()->addMenu("&Help");
    helpMenu->addAction(helpAction);
    helpMenu->addAction(infoAction);
    helpMenu->addAction(aboutAction);
}

void Ui::createToolBars()
{
    QToolBar* mainToolBar = addToolBar("Main");
    mainToolBar->addAction(newQueryAction);
    mainToolBar->addAction(openDbAction);
    mainToolBar->addAction(runQueryAction);
    mainToolBar->addAction(helpAction);
    mainToolBar->addAction(infoAction);
}

void Ui::createStatusBar()
{
    statusBar()->showMessage("Ready");
}

void Ui::createDocks()
{
    QDockWidget* dbExplorerDock = new QDockWidget("Database Explorer", this);
    dbExplorerDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    dbExplorerDock->setStyleSheet(R"( QDockWidget::title { text-align: left; padding: 6px; font-size: 16px; font-weight: bold; } )");

    dbExplorerView = new QTreeView(dbExplorerDock);
    dbExplorerModel = new QStandardItemModel(dbExplorerDock);
    dbExplorerView->setModel(dbExplorerModel);
    dbExplorerView->setHeaderHidden(true);
    connect(dbExplorerView, &QTreeView::clicked, this, &Ui::onTableClicked);
    dbExplorerView->setStyleSheet(
        R"( QTreeView { background-color: #252526; color: #CCCCCC; border: none; } QTreeView::item { padding: 8px; font-size: 16px; } QTreeView::item:hover { background-color: #3E3E42; } QTreeView::item:selected { background-color: #094771; } )");

    dbExplorerDock->setWidget(dbExplorerView);
    addDockWidget(Qt::LeftDockWidgetArea, dbExplorerDock);
}

void Ui::updateDatabaseExplorer()
{
    dbExplorerModel->clear();
    QStandardItem* root = dbExplorerModel->invisibleRootItem();
    for (const auto& tableName : dbms.get_table_names()) {
        auto* tableItem = new QStandardItem(QString::fromStdString(tableName));
        tableItem->setIcon(QIcon::fromTheme("database"));
        tableItem->setEditable(false);
        root->appendRow(tableItem);
    }
    updateCompleterContext();
}

QueryTab* Ui::currentQueryTab()
{
    return qobject_cast<QueryTab*>(tabWidget->currentWidget());
}

CodeEditor* Ui::currentQueryEdit()
{
    if (QueryTab* tab = currentQueryTab()) {
        return tab->getEditor();
    }
    return nullptr;
}

void Ui::onNewQueryTab(bool isConsole)
{
    auto* newTab = new QueryTab(completer, this);
    connect(newTab->getEditor(), &CodeEditor::cursorPositionChanged, this, &Ui::updateCompleterContext);

    int newIndex;
    if (isConsole) {
        newIndex = tabWidget->insertTab(0, newTab, "Console");
    }
    else {
        newIndex = tabWidget->addTab(newTab, QString("Query %1").arg(tabWidget->count() + 1));
    }

    tabWidget->setCurrentIndex(newIndex);
    newTab->getEditor()->setFocus();
}

void Ui::closeTab(int index) const
{
    if (index == 0) {
        return;
    }

    QWidget* tab = tabWidget->widget(index);
    tabWidget->removeTab(index);
    tab->deleteLater();
}

void Ui::onTableClicked(const QModelIndex& index)
{
    if (!index.parent().isValid()) {
        QString tableName = dbExplorerModel->data(index).toString();
        onNewQueryTab(false);
        if (auto* editor = currentQueryEdit()) {
            editor->setText(QString("SELECT * FROM %1;").arg(tableName));
            runQueryAction->trigger();
        }
    }
}

void Ui::onOpenDatabase()
{
    QString dir = QFileDialog::getExistingDirectory(this, "Open Database Directory", "saves/tables");
    if (dir.isEmpty()) {
        return;
    }
    if (dbms.load_database_from_path(dir.toStdString())) {
        statusBar()->showMessage("Database loaded successfully!", 4000);
        updateDatabaseExplorer();
    }
    else {
        QMessageBox::warning(this, "Error", "Failed to load database from the selected directory.");
    }
}

void Ui::onRunQuery()
{
    QueryTab* currentTab = currentQueryTab();
    if (!currentTab) {
        return;
    }

    CodeEditor* editor = currentTab->getEditor();
    QString textToExecute = editor->textCursor().hasSelection()
                                ? editor->textCursor().selectedText()
                                : editor->toPlainText();

    if (textToExecute.trimmed().isEmpty()) {
        statusBar()->showMessage("No query to execute.", 3000);
        return;
    }

    QRegularExpression separator(";\\s*|\\n\\s*\\n");
    QStringList queries = textToExecute.split(separator, Qt::SkipEmptyParts);

    statusBar()->showMessage(QString("Found %1 queries to execute...").arg(queries.size()));

    for (int i = 0; i < queries.size(); ++i) {
        const QString& queryStr = queries[i];
        if (queryStr.trimmed().isEmpty()) {
            continue;
        }

        QueryResult result = dbms.process_query(queryStr.trimmed().toStdString());

        currentTab->displayResult(result, queryStr.trimmed());

        QApplication::processEvents();

        QString upperQuery = queryStr.trimmed().toUpper();
        if (upperQuery.startsWith("CREATE") || upperQuery.startsWith("DROP")) {
            updateDatabaseExplorer();
        }
    }
    statusBar()->showMessage("All queries finished.", 3000);
}

void Ui::about()
{
    QMessageBox::about(this, "About DBMS",
                       "A simple yet powerful SQL database management system built with C++ and Qt.\n\nCreated by Arsenii Zarudniuk.");
}

void Ui::onHelp()
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == "Help") {
            tabWidget->setCurrentIndex(i);
            return;
        }
    }

    auto* helpWidget = new HelpWidget(this);
    int helpIndex = tabWidget->addTab(helpWidget, "Help");
    tabWidget->setCurrentIndex(helpIndex);
}

void Ui::onInfo()
{
    for (int i = 0; i < tabWidget->count(); ++i) {
        if (tabWidget->tabText(i) == "Info") {
            tabWidget->setCurrentIndex(i);
            return;
        }
    }

    auto* infoWidget = new InfoWidget(this);
    int infoIndex = tabWidget->addTab(infoWidget, "Info");
    tabWidget->setCurrentIndex(infoIndex);
}

void Ui::updateCompleterContext()
{
    QueryTab* currentTab = currentQueryTab();
    if (!currentTab) {
        return;
    }

    CodeEditor* editor = currentTab->getEditor();
    if (!editor || !completer) {
        return;
    }

    QTextCursor cursor = editor->textCursor();
    cursor.movePosition(QTextCursor::StartOfWord, QTextCursor::MoveAnchor);
    cursor.movePosition(QTextCursor::PreviousWord, QTextCursor::MoveAnchor);
    cursor.select(QTextCursor::WordUnderCursor);
    QString prevWord = cursor.selectedText().toUpper();

    QStringList keywords;
    QStringList tableNames;
    for (const auto& name : dbms.get_table_names()) {
        tableNames.append(QString::fromStdString(name));
    }

    if (prevWord == "FROM" || prevWord == "TABLE" || prevWord == "INTO" || prevWord == "UPDATE") {
        keywords = tableNames;
    }
    else {
        keywords << "SELECT" << "FROM" << "WHERE" << "INSERT" << "INTO" << "VALUES"
            << "UPDATE" << "SET" << "DELETE" << "CREATE" << "TABLE" << "DROP"
            << "GROUP BY" << "HAVING" << "COUNT";
        keywords.append(tableNames);
    }
    completerModel->setStringList(keywords);
}
