#ifndef QUERYTAB_HPP
#define QUERYTAB_HPP

#include <QWidget>
#include <variant>
#include <vector>

class CodeEditor;
class QCompleter;
class QTableView;
class QTextEdit;
class SqlTableModel;
struct Column;

using TabQueryResult = std::variant<std::string, std::vector<Column>>;

class QueryTab : public QWidget
{
    Q_OBJECT
public:
    explicit QueryTab(QCompleter* completer, QWidget *parent = nullptr);
    ~QueryTab() override;

    CodeEditor* getEditor() const;
    void displayResult(const TabQueryResult& result, const QString& query);

private:
    CodeEditor* queryEditor;
    QTableView* tableView;
    QTextEdit* messageEdit;
    SqlTableModel* tableModel = nullptr;
};

#endif // QUERYTAB_HPP
