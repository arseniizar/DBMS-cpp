#include "querytab.hpp"
#include "codeeditor.hpp"
#include "sqltablemodel.hpp"
#include "engine/table/structs/column/Column.hpp"
#include "sqlhighlighter.hpp"

#include <QHeaderView>
#include <QSplitter>
#include <QTableView>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QTabWidget>
#include <QTabBar>
#include <QRegularExpression>

QueryTab::QueryTab(QCompleter* completer, QWidget* parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QSplitter* splitter = new QSplitter(Qt::Vertical, this);

    queryEditor = new CodeEditor(splitter);
    new SqlHighlighter(queryEditor->document());
    if (completer) {
        queryEditor->setCompleter(completer);
    }

    resultsTabWidget = new QTabWidget(splitter);
    resultsTabWidget->setTabBarAutoHide(true);

    splitter->addWidget(queryEditor);
    splitter->addWidget(resultsTabWidget);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter);

    auto* initialLabel = new QTextEdit();
    initialLabel->setReadOnly(true);
    initialLabel->setText("Enter a query and press F5 or the Run button to execute.");
    initialLabel->setProperty("isInitial", true);
    resultsTabWidget->addTab(initialLabel, "Messages");
}

QueryTab::~QueryTab() {}

CodeEditor* QueryTab::getEditor() const
{
    return queryEditor;
}

void QueryTab::clearResultTabs()
{
    resultsTabWidget->clear();
}

void QueryTab::displayResult(const TabQueryResult& result, const QString& query)
{
    if (resultsTabWidget->count() == 1) {
        QWidget* widget = resultsTabWidget->widget(0);
        if (widget && widget->property("isInitial").toBool()) {
            resultsTabWidget->removeTab(0);
        }
    }

    QRegularExpression re(R"((?:FROM|INTO|UPDATE|TABLE)\s+([a-zA-Z0-9_]+))", QRegularExpression::CaseInsensitiveOption);
    QRegularExpressionMatch match = re.match(query);
    QString tableName = match.hasMatch() ? match.captured(1) : "";

    QString tabName;
    QString upperQuery = query.toUpper();

    if (upperQuery.startsWith("SELECT")) {
        tabName = tableName.isEmpty() ? "Result Set" : QString("Result: %1").arg(tableName);
    } else if (upperQuery.startsWith("CREATE TABLE")) {
        tabName = tableName.isEmpty() ? "Create" : QString("Create: %1").arg(tableName);
    } else if (upperQuery.startsWith("INSERT")) {
        tabName = tableName.isEmpty() ? "Insert" : QString("Insert: %1").arg(tableName);
    } else {
        tabName = "Message";
    }


    std::visit([this, &query, &tabName](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            auto* messageEdit = new QTextEdit();
            messageEdit->setReadOnly(true);
            messageEdit->setPlainText(QString("Query:\n%1\n\nResult:\n%2").arg(query, QString::fromStdString(arg)));
            resultsTabWidget->addTab(messageEdit, "Message");

        } else if constexpr (std::is_same_v<T, std::vector<Column>>) {
            if (arg.empty() || arg[0].get_rows().empty()) {
                auto* messageEdit = new QTextEdit();
                messageEdit->setReadOnly(true);
                messageEdit->setPlainText(QString("Query:\n%1\n\nResult:\nQuery executed successfully, no rows returned.").arg(query));
                resultsTabWidget->addTab(messageEdit, "Message");
            } else {
                auto* tableView = new QTableView();
                tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
                tableView->setAlternatingRowColors(true);
                tableView->setModel(new SqlTableModel(arg, tableView));
                resultsTabWidget->addTab(tableView, tabName);
            }
        }
    }, result);

    resultsTabWidget->setCurrentIndex(resultsTabWidget->count() - 1);
}
