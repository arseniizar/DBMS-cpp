#ifndef UI_HPP
#define UI_HPP

#include <QMainWindow>
#include "engine/Dbms/Dbms.hpp"

class QAction;
class QCompleter;
class QStringListModel;
class QTabWidget;
class QTreeView;
class QStandardItemModel;
class CodeEditor;
class QueryTab;
class HelpWidget;
class InfoWidget;

class Ui final : public QMainWindow
{
    Q_OBJECT
public:
    explicit Ui(QWidget* parent = nullptr);
    ~Ui() override;

private slots:
    void onOpenDatabase();
    void onRunQuery();
    void onHelp();
    void onInfo();
    void about();
    void onNewQueryTab(bool isConsole = false);
    void closeTab(int index) const;
    void onTableClicked(const QModelIndex& index);
    void updateCompleterContext();

private:
    void setupUi();
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDocks();
    void applyDarkTheme();
    void setupCompleter();
    void updateDatabaseExplorer();

    QueryTab* currentQueryTab();
    CodeEditor* currentQueryEdit();

    Dbms dbms;

    // Widgets
    QTabWidget*       tabWidget;
    QTreeView*        dbExplorerView;
    QStandardItemModel* dbExplorerModel;
    QCompleter*       completer;
    QStringListModel* completerModel;

    // Actions
    QAction*          newQueryAction;
    QAction*          openDbAction;
    QAction*          runQueryAction;
    QAction*          helpAction;
    QAction*          infoAction;
    QAction*          exitAction;
    QAction*          aboutAction;
};

#endif // UI_HPP
