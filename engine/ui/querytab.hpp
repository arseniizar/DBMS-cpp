#ifndef QUERYTAB_HPP
#define QUERYTAB_HPP

#include <QWidget>
#include <variant>
#include <vector>

class CodeEditor;
class QCompleter;
class QTabWidget;
class SqlTableModel;
struct Column;

using TabQueryResult = std::variant<std::string, std::vector<Column>>;

class QueryTab : public QWidget
{
    Q_OBJECT
public:
    explicit QueryTab(QCompleter* completer, QWidget* parent = nullptr);
    ~QueryTab() override;

    CodeEditor* getEditor() const;
    void displayResult(const TabQueryResult& result, const QString& query);
    void clearResultTabs();

private:
    CodeEditor* queryEditor;
    QTabWidget* resultsTabWidget;
};

#endif // QUERYTAB_HPP
