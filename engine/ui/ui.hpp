#ifndef UI_HPP
#define UI_HPP

#include <QMainWindow>
#include <Dbms/Dbms.hpp>

class QTextEdit;
class QPushButton;

struct Ui final : QMainWindow {
    Q_OBJECT
public:
    explicit Ui(QWidget *parent = nullptr);
    ~Ui() override;
private:
    QTextEdit *queryEdit;
    QPushButton *runButton;
    QPushButton *listTablesButton;
    QPushButton *helpButton;
    QPushButton *loadButton;
    QTextEdit *outputEdit;
    Dbms dbms;
private slots:
    void onRunButtonClicked();
    void onListTablesClicked();
    void onHelpButtonClicked();
    void onLoadButtonClicked();
};

#endif // UI_HPP
