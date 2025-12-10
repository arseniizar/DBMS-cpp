#include "querytab.hpp"
#include "codeeditor.hpp"
#include "sqltablemodel.hpp"
#include "engine/table/structs/column/Column.hpp"

#include <QVBoxLayout>
#include <QSplitter>
#include <QHeaderView>
#include <QTableView>

#include "sqlhighlighter.hpp"

QueryTab::QueryTab(QCompleter* completer, QWidget *parent) : QWidget(parent)
{
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QSplitter *splitter = new QSplitter(Qt::Vertical, this);

    queryEditor = new CodeEditor(splitter);
    new SqlHighlighter(queryEditor->document());
    if (completer) {
        queryEditor->setCompleter(completer);
    }

    QWidget *resultsContainer = new QWidget(splitter);
    auto* resultsLayout = new QVBoxLayout(resultsContainer);
    resultsLayout->setContentsMargins(0, 0, 0, 0);

    tableView = new QTableView();
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    tableView->setAlternatingRowColors(true);

    messageEdit = new QTextEdit();
    messageEdit->setReadOnly(true);

    resultsLayout->addWidget(tableView);
    resultsLayout->addWidget(messageEdit);

    splitter->addWidget(queryEditor);
    splitter->addWidget(resultsContainer);
    splitter->setStretchFactor(0, 1);
    splitter->setStretchFactor(1, 1);

    layout->addWidget(splitter);

    tableView->setVisible(false);
    messageEdit->setVisible(true);
    messageEdit->setText("Enter a query and press F5 or the Run button to execute.");
}

QueryTab::~QueryTab()
{
    delete tableModel;
}

CodeEditor* QueryTab::getEditor() const {
    return queryEditor;
}

void QueryTab::displayResult(const TabQueryResult& result, const QString& query)
{
    delete tableModel;
    tableModel = nullptr;
    tableView->setModel(nullptr);
    messageEdit->clear();

    std::visit([this, &query](auto&& arg) {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::string>) {
            messageEdit->setPlainText(QString("Query:\n%1\n\nResult:\n%2").arg(query, QString::fromStdString(arg)));
            tableView->setVisible(false);
            messageEdit->setVisible(true);
        } else if constexpr (std::is_same_v<T, std::vector<Column>>) {
            if (arg.empty() || arg[0].get_rows().empty()) {
                messageEdit->setPlainText(QString("Query:\n%1\n\nResult:\nQuery executed successfully, no rows returned.").arg(query));
                tableView->setVisible(false);
                messageEdit->setVisible(true);
            } else {
                tableModel = new SqlTableModel(arg, this);
                tableView->setModel(tableModel);
                messageEdit->setVisible(false);
                tableView->setVisible(true);
            }
        }
    }, result);
}
