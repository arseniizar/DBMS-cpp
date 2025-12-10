#ifndef UI_HPP
#define UI_HPP

#include <QMainWindow>
#include "engine/Dbms/Dbms.hpp"
#include "sqltablemodel.hpp"
#include "sqlhighlighter.hpp"

class QAction;
class QMenu;
class QToolBar;
class QStatusBar;
class QTextEdit;
class QTableView;
class QTabWidget;

class Ui final : public QMainWindow {
    Q_OBJECT
public:
    explicit Ui(QWidget *parent = nullptr);
    ~Ui() override;

private slots:
    void onRunQuery();
    void onListTables();
    void onHelp();
    void onLoad();
    void about();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDocks();

    Dbms dbms;
    SqlTableModel *model;
    SqlHighlighter *highlighter;

    QTabWidget *tabWidget;
    QTableView *tableView;
    QTextEdit *messageEdit;

    QMenu *fileMenu;
    QMenu *queryMenu;
    QMenu *helpMenu;

    QToolBar *mainToolBar;

    QAction *runQueryAction;
    QAction *listTablesAction;
    QAction *helpAction;
    QAction *loadAction;
    QAction *exitAction;
    QAction *aboutAction;

    QTextEdit* currentQueryEdit();
};

#endif // UI_HPP
