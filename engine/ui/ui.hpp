#ifndef UI_HPP
#define UI_HPP

#include <QMainWindow>
#include "engine/Dbms/Dbms.hpp"
#include "sqltablemodel.hpp"
#include "sqlhighlighter.hpp"

class CodeEditor;
class QAction;
class QTextEdit;
class QTableView;
class QTabWidget;
class QTreeView;
class QStandardItemModel;
class QCompleter;
class QStringListModel;

class Ui final : public QMainWindow {
    Q_OBJECT
public:
    explicit Ui(QWidget *parent = nullptr);
    ~Ui() override;

private slots:
    void onOpenDatabase();
    void onRunQuery();
    void onHelp();
    void about();
    void onNewQueryTab();
    void onTableClicked(const QModelIndex &index);

private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDocks();
    void applyDarkTheme();
    void updateDatabaseExplorer();
    void setupCompleter();

    CodeEditor* currentQueryEdit();

    Dbms dbms;
    SqlTableModel *model;

    QTabWidget *tabWidget;
    QTableView *tableView;
    QTextEdit *messageEdit;
    QTreeView *dbExplorerView;
    QStandardItemModel *dbExplorerModel;

    QAction *newQueryAction;
    QAction *openDbAction;
    QAction *runQueryAction;
    QAction *helpAction;
    QAction *exitAction;
    QAction *aboutAction;

    QCompleter *completer;
    QStringListModel *completerModel;
};

#endif // UI_HPP
